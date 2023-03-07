#pragma once

namespace trnr::core::lib {

enum state {
    Idle = 0,
    Attack1,
    Attack2,
    Hold,
    Decay1,
    Decay2,
    Sustain,
    Release1,
    Release2
};

class tx_envelope {

public:
    float attackRate;
    float attackCurvature;
    float holdRate;
    float decayRate;
    float decayLevel;
    float sustainLevel;
    float releaseRate;
    float releaseCurvature;

    tx_envelope(double sampleRate)
        : sampleRate { sampleRate }
        , attackRate { 0 }
        , attackCurvature { 0 }
        , holdRate { 0 }
        , decayRate { 0 }
        , decayLevel { 0 }
        , sustainLevel { 0 }
        , releaseRate { 0 }
        , releaseCurvature { 0 }
        , level { 0.f }
        , phase { 0 }
        , state { Idle }
        , startLevel { 0.f }
        , h1 { 0. }
        , h2 { 0. }
        , h3 { 0. }
    {
    }

    float processSample(bool gate, bool trigger) {

        float attackSamp = msToSamples(attackRate);
        int holdSamp = msToSamples(holdRate);
        float decaySamp = msToSamples(decayRate);
        float releaseSamp = msToSamples(releaseRate);

        int attackMidX1 = static_cast<int>((1 - attackCurvature) * attackSamp);
        int attackMidX2 = static_cast<int>(attackCurvature * attackSamp);
        int decayMidX1 = static_cast<int>(decayLevel * decaySamp);
        int decayMidX2 = static_cast<int>((1 - decayLevel) * decaySamp);
        int releaseMidX1 = static_cast<int>(releaseCurvature * releaseSamp);
        int releaseMidX2 = static_cast<int>((1 - releaseCurvature) * releaseSamp);

        // if note on is triggered, transition to attack phase
        if (trigger) {
            startLevel = 0.f;
            phase = 0;
            state = Attack1;
        }
        // attack 1st half
        if (state == Attack1) {
            // while in attack phase
            if (phase < attackMidX1) {
                level = lerp(0, startLevel, attackMidX1, attackCurvature, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > attackMidX1) {
                phase = attackMidX1;
            }
            // if attack phase is done, transition to decay phase
            if (phase == attackMidX1) {
                state = Attack2;
                phase = 0;
            }
        }
        // attack 2nd half
        if (state == Attack2) {
            // while in attack phase
            if (phase < attackMidX2) {
                level = lerp(0, attackCurvature, attackMidX2, 1, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > attackMidX2) {
                phase = attackMidX2;
            }
            // if attack phase is done, transition to decay phase
            if (phase == attackMidX2) {
                state = Hold;
                phase = 0;
            }
        }
        // hold
        if (state == Hold) {
            if (phase < holdSamp) {
                level = 1.0;
                phase += 1;
            }
            if (phase > holdSamp) {
                phase = holdSamp;
            }
            if (phase == holdSamp) {
                state = Decay1;
                phase = 0;
            }
        }
        // decay 1st half
        if (state == Decay1) {
            // while in decay phase
            if (phase < decayMidX1) {
                level = lerp(0, 1, decayMidX1, decayLevel, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > decayMidX1) {
                phase = decayMidX1;
            }
            // if decay phase is done, transition to sustain phase
            if (phase == decayMidX1) {
                state = Decay2;
                phase = 0;
            }
        }
        // decay 2nd half
        if (state == Decay2) {
            // while in decay phase
            if (phase < decayMidX2) {
                level = lerp(0, decayLevel, decayMidX2, sustainLevel, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > decayMidX2) {
                phase = decayMidX2;
            }
            // if decay phase is done, transition to sustain phase
            if (phase == decayMidX2) {
                state = Sustain;
                phase = 0;
                level = sustainLevel;
            }
        }
        // while sustain phase: if note off is triggered, transition to release phase
        if (state == Sustain && !gate) {
            state = Release1;
            level = sustainLevel;
        }
        // release 1st half
        if (state == Release1) {
            // while in release phase
            if (phase < releaseMidX1) {
                level = lerp(0, sustainLevel, releaseMidX1, releaseCurvature, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > releaseMidX1) {
                phase = releaseMidX1;
            }
            // transition to 2nd release half
            if (phase == releaseMidX1) {
                phase = 0;
                state = Release2;
            }
        }
        // release 2nd half
        if (state == Release2) {
            // while in release phase
            if (phase < releaseMidX2) {
                level = lerp(0, releaseCurvature, releaseMidX2, 0, phase);
                phase += 1;
            }
            // reset phase if parameter was changed
            if (phase > releaseMidX2) {
                phase = releaseMidX2;
            }
            // reset
            if (phase == releaseMidX2) {
                phase = 0;
                state = Idle;
                level = 0;
            }
        }

        return smooth(level);
    }

    bool isBusy() { return state != 0; }

    void setSampleRate(double sampleRate) {
        this->sampleRate = sampleRate;
    }
    
private:
    double sampleRate;
    int phase;
    float level;
    state state;
    float startLevel;
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

    float msToSamples(float ms) { return ms * sampleRate / 1000.f; }
};   
}