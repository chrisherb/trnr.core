#pragma once
#include <array>

namespace trnr::core::lib {

enum state {
    idle = 0,
    attack1,
    attack2,
    hold,
    decay1,
    decay2,
    sustain,
    release1,
    release2
};

class tx_envelope {
public:
    float attack1_rate;
    float attack1_level;
    float attack2_rate;
    float hold_rate;
    float decay1_rate;
    float decay1_level;
    float decay2_rate;
    float sustain_level;
    float release1_rate;
    float release1_level;
    float release2_rate;

    tx_envelope(double _samplerate)
        : samplerate { _samplerate }
        , attack1_rate { 0 }
        , attack1_level { 0 }
        , attack2_rate { 0 }
        , hold_rate { 0 }
        , decay1_rate { 0 }
        , decay1_level { 0 }
        , decay2_rate { 0 }
        , sustain_level { 0 }
        , release1_rate { 0 }
        , release1_level { 0 }
        , release2_rate { 0 }
        , level { 0.f }
        , phase { 0 }
        , state { idle }
        , start_level { 0.f }
        , h1 { 0. }
        , h2 { 0. }
        , h3 { 0. }
    {
    }

    float process_sample(bool gate, bool trigger) {

        int attack_mid_x1 = ms_to_samples(attack1_rate);
        int attack_mid_x2 = ms_to_samples(attack2_rate);
        int hold_samp = ms_to_samples(hold_rate);
        int decay_mid_x1 = ms_to_samples(decay1_rate);
        int decay_mid_x2 = ms_to_samples(decay2_rate);
        int release_mid_x1 = ms_to_samples(release1_rate);
        int release_mid_x2 = ms_to_samples(release2_rate);

        // if note on is triggered, transition to attack phase
        if (trigger) {
            start_level = level;
            phase = 0;
            state = attack1;
        }
        // attack 1st half
        if (state == attack1) {
            // while in attack phase
            if (phase < attack_mid_x1) {
                level = lerp(0, start_level, attack_mid_x1, attack1_level, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > attack_mid_x1) {
                phase = attack_mid_x1;
            }
            // if attack phase is done, transition to decay phase
            if (phase == attack_mid_x1) {
                state = attack2;
                phase = 0;
            }
        }
        // attack 2nd half
        if (state == attack2) {
            // while in attack phase
            if (phase < attack_mid_x2) {
                level = lerp(0, attack1_level, attack_mid_x2, 1, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > attack_mid_x2) {
                phase = attack_mid_x2;
            }
            // if attack phase is done, transition to decay phase
            if (phase == attack_mid_x2) {
                state = hold;
                phase = 0;
            }
        }
        // hold
        if (state == hold) {
            if (phase < hold_samp) {
                level = 1.0;
                phase += 1;
            }
            if (phase > hold_samp) {
                phase = hold_samp;
            }
            if (phase == hold_samp) {
                state = decay1;
                phase = 0;
            }
        }
        // decay 1st half
        if (state == decay1) {
            // while in decay phase
            if (phase < decay_mid_x1) {
                level = lerp(0, 1, decay_mid_x1, decay1_level, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > decay_mid_x1) {
                phase = decay_mid_x1;
            }
            // if decay phase is done, transition to sustain phase
            if (phase == decay_mid_x1) {
                state = decay2;
                phase = 0;
            }
        }
        // decay 2nd half
        if (state == decay2) {
            // while in decay phase
            if (phase < decay_mid_x2) {
                level = lerp(0, decay1_level, decay_mid_x2, sustain_level, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > decay_mid_x2) {
                phase = decay_mid_x2;
            }
            // if decay phase is done, transition to sustain phase
            if (phase == decay_mid_x2) {
                state = sustain;
                phase = 0;
                level = sustain_level;
            }
        }
        // while sustain phase: if note off is triggered, transition to release phase
        if (state == sustain && !gate) {
            state = release1;
            level = sustain_level;
        }
        // release 1st half
        if (state == release1) {
            // while in release phase
            if (phase < release_mid_x1) {
                level = lerp(0, sustain_level, release_mid_x1, release1_level, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > release_mid_x1) {
                phase = release_mid_x1;
            }
            // transition to 2nd release half
            if (phase == release_mid_x1) {
                phase = 0;
                state = release2;
            }
        }
        // release 2nd half
        if (state == release2) {
            // while in release phase
            if (phase < release_mid_x2) {
                level = lerp(0, release1_level, release_mid_x2, 0, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > release_mid_x2) {
                phase = release_mid_x2;
            }
            // reset
            if (phase == release_mid_x2) {
                phase = 0;
                state = idle;
                level = 0;
            }
        }

        return smooth(level);
    }

    bool is_busy() { return state != 0; }

    void set_samplerate(double sampleRate) {
        this->samplerate = sampleRate;
    }

    // returns the x/y coordinates of the envelope points as a list for graphical representation.
    std::array<float, 18> calc_coordinates() {

        float a_x = 0;
        float a_y = 0;

        float b_x = attack1_rate;
        float b_y = attack1_level;

        float c_x = b_x + attack2_rate;
        float c_y = 1;

        float d_x = c_x + hold_rate;
        float d_y = 1;
        
        float e_x = d_x + decay1_rate;
        float e_y = decay1_level;

        float f_x = e_x + decay2_rate;
        float f_y = sustain_level;

        float g_x = f_x + 500;
        float g_y = sustain_level;

        float h_x = g_x + release1_rate;
        float h_y = release1_level;

        float i_x = h_x + release2_rate;
        float i_y = 0;

        float total = i_x;

        return {
            a_x,
            a_y,
            b_x / total,
            b_y,
            c_x / total,
            c_y,
            d_x / total,
            d_y,
            e_x / total,
            e_y,
            f_x / total,
            f_y,
            g_x / total,
            g_y,
            h_x / total,
            h_y,
            i_x / total,
            i_y
        };
    }
    
private:
    double samplerate;
    int phase;
    float level;
    state state;
    float start_level;
    float h1;
    float h2;
    float h3;

    float lerp(float x1, float y1, float x2, float y2, float x) { return y1 + (((x - x1) * (y2 - y1)) / (x2 - x1)); }

    float smooth(float sample) {
        h3 = h2;
        h2 = h1;
        h1 = sample;

        return (h1 + h2 + h3) / 3.f;
    }

    float ms_to_samples(float ms) { return ms * samplerate / 1000.f; }
};
}