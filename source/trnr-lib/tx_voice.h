#pragma once
#include "tx_sineosc.h"
#include "tx_envelope.h"
#include "tx_operator.h"

namespace trnr::core::lib {

class tx_voice {
public:
    tx_voice(double samplerate) 
        : algorithm { 0 }
        , pitch_env_amt { 0.f }
        , feedback_amt { 0.f }
        , pitch_env(samplerate)
        , feedback_osc(samplerate)
        , op1(samplerate)
        , op2(samplerate)
        , op3(samplerate)
    {
    }

    bool gate = false;
    bool trigger = false;
    float frequency = 100.f;
    float velocity = 1.f;

    int algorithm;
    float pitch_env_amt;
    float feedback_amt;
    tx_sineosc feedback_osc;
    tx_envelope pitch_env;
    tx_operator op1;
    tx_operator op2;
    tx_operator op3;

    float process_sample() {
        float pitch_env_signal = pitch_env.process_sample(gate, trigger) * pitch_env_amt;
        float pitched_freq = frequency + pitch_env_signal;

        float output = 0.f;

        // mix operator signals according to selected algorithm
        switch (algorithm) {
        case 0:
            output = calc_algo1(pitched_freq);
            break;
        case 1:
            output = calc_algo2(pitched_freq);
            break;
        case 2:
            output = calc_algo3(pitched_freq);
            break;
        case 3:
            output = calc_algo4(pitched_freq);
            break;
        default:
            output = calc_algo1(pitched_freq);
            break;
        }

        // reset trigger
        trigger = false;

        return output;
    }

    bool is_busy() { return gate || op1.envelope.is_busy() || op2.envelope.is_busy() || op3.envelope.is_busy(); }

    void set_samplerate(double samplerate) {
        pitch_env.set_samplerate(samplerate);
        feedback_osc.setSampleRate(samplerate);
        op1.set_samplerate(samplerate);
        op2.set_samplerate(samplerate);
        op3.set_samplerate(samplerate);
    }

    void set_phase_reset(bool phase_reset) {
        op1.oscillator.phase_reset = phase_reset;
        op2.oscillator.phase_reset = phase_reset;
        op3.oscillator.phase_reset = phase_reset;
        feedback_osc.phase_reset = phase_reset;
    }

private:
    const float MOD_INDEX_COEFF = 4.f;

    float calc_algo1(const float frequency) {
        float fb_freq = frequency * op3.ratio;
        float fb_mod_index = (feedback_amt * MOD_INDEX_COEFF);
        float fb_signal = feedback_osc.process_sample(trigger, fb_freq) * fb_mod_index;

        float op3_Freq = frequency * op3.ratio;
        float op3_mod_index = (op3.amplitude * MOD_INDEX_COEFF);
        float op3_signal = op3.process_sample(gate, trigger, op3_Freq, velocity, fb_signal) * op3_mod_index;

        float op2_freq = frequency * op2.ratio;
        float op2_mod_index = (op2.amplitude * MOD_INDEX_COEFF);
        float op2_signal = op2.process_sample(gate, trigger, op2_freq, velocity, op3_signal) * op2_mod_index;

        float op1_freq = frequency * op1.ratio;
        return op1.process_sample(gate, trigger, op1_freq, velocity, op2_signal) * op1.amplitude;
    }

    float calc_algo2(const float frequency) {
        float fb_freq = frequency * op3.ratio;
        float fb_mod_index = (feedback_amt * MOD_INDEX_COEFF);
        float fb_signal = feedback_osc.process_sample(trigger, fb_freq) * fb_mod_index;

        float op3_freq = frequency * op3.ratio;
        float op3_signal = op3.process_sample(gate, trigger, op3_freq, velocity, fb_signal) * op3.amplitude;

        float op2_freq = frequency * op2.ratio;
        float op2_mod_index = (op2.amplitude * MOD_INDEX_COEFF);
        float op2_signal = op2.process_sample(gate, trigger, op2_freq, velocity) * op2_mod_index;

        float op1_freq = frequency * op1.ratio;
        float op1_signal = op1.process_sample(gate, trigger, op1_freq, velocity, op2_signal) * op1.amplitude;

        return op1_signal + op3_signal;
    }

    float calc_algo3(const float frequency) {
        float fb_freq = frequency * op3.ratio;
        float fb_mod_index = (feedback_amt * MOD_INDEX_COEFF);
        float fb_signal = feedback_osc.process_sample(trigger, fb_freq) * fb_mod_index;

        float op3_freq = frequency * op3.ratio;
        float op3_signal = op3.process_sample(gate, trigger, op3_freq, velocity, fb_signal) * op3.amplitude;

        float op2_freq = frequency * op2.ratio;
        float op2_signal = op2.process_sample(gate, trigger, op2_freq, velocity) * op2.amplitude;

        float op1_freq = frequency * op1.ratio;
        float op1_signal = op1.process_sample(gate, trigger, op1_freq, velocity) * op1.amplitude;

        return op1_signal + op2_signal + op3_signal;
    }

    float calc_algo4(const float frequency) {
        float fb_freq = frequency * op3.ratio;
        float fb_mod_index = (feedback_amt * MOD_INDEX_COEFF);
        float fb_signal = feedback_osc.process_sample(trigger, fb_freq) * fb_mod_index;

        float op3_freq = frequency * op3.ratio;
        float op3_mod_index = (op3.amplitude * MOD_INDEX_COEFF);
        float op3_signal = op3.process_sample(gate, trigger, op3_freq, velocity, fb_signal) * op3_mod_index;

        float op2_freq = frequency * op2.ratio;
        float op2_mod_index = (op2.amplitude * MOD_INDEX_COEFF);
        float op2_signal = op2.process_sample(gate, trigger, op2_freq, velocity) * op2_mod_index;

        float op1_freq = frequency * op1.ratio;
        return op1.process_sample(gate, trigger, op1_freq, velocity, op2_signal + op3_signal) * op1.amplitude;
    }
};
}