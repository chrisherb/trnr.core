#include "c74_min.h"
#include "../../trnr-lib/tx_voice.h"
#include "../../trnr-lib/ylowpass.h"

using namespace c74::min;
using namespace trnr::core::lib;

class txc : public object<txc>, public vector_operator<> {
private:
    bool initialized;
public:
    MIN_DESCRIPTION	{"FM synth with macro controls"};
    MIN_TAGS		{"synth voice"};
    MIN_AUTHOR		{"Christopher Herb"};

	inlet<> in1 {this, "(bang) trigger, (int) gate"};
	outlet<> out1 {this, "(signal) Output", "signal"};

    message<> setup {this, "setup",
        MIN_FUNCTION {
            triplex_voice = std::make_unique<tx_voice>(0);
            triplex_voice->op1.amplitude = 1;
            initialized = true;
            return {};
       }
    };

    message<> dspsetup {this, "dspsetup",
        MIN_FUNCTION {
            triplex_voice->set_samplerate(samplerate());
            return {};
       }
    };

    message<threadsafe::yes> trigger { this, "bang", "Trigger",
        MIN_FUNCTION {
            triplex_voice->trigger = true;
            return {};
        }
    };

    message<threadsafe::yes> gate { this, "int", "Gate on/off",
        MIN_FUNCTION {
            int arg = args[0];
            if (arg > 0) {
                triplex_voice->gate = true;
            } else {
                triplex_voice->gate = false;
            }
            return {};
        }
    };

    attribute<number, threadsafe::yes, limit::clamp> velocity { this, "velocity", 100, range { 0, 127 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                double vel = args[0];
                triplex_voice->velocity = vel / 127.0;
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::yes, limit::clamp> frequency { this, "frequency", 100, range { 20, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->frequency = args[0];
            }
            return args;
        }}
    };

    attribute<number, threadsafe::yes, limit::clamp> fm { this, "fm", 0.2, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                float amp = scale(args[0], 0, 1, 2);
                triplex_voice->op2.amplitude = amp;
                triplex_voice->op3.amplitude = amp;
                triplex_voice->feedback_amt = amp;
            }
            return args;
        }}
    };

    attribute<number, threadsafe::yes, limit::clamp> harmonicity { this, "harmonicity", 1, range { 0, 9 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.ratio = args[0];
                triplex_voice->op3.ratio = args[0];
            }
            return args;
        }}
    };

    attribute<number, threadsafe::yes, limit::clamp> redux { this, "redux", 0, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                float phase_res = scale(args[0], 16, 2, 0.25);
                triplex_voice->op1.oscillator.set_phase_resolution(phase_res);
                triplex_voice->op2.oscillator.set_phase_resolution(phase_res);
                triplex_voice->op3.oscillator.set_phase_resolution(phase_res);
                triplex_voice->feedback_osc.set_phase_resolution(phase_res);
                triplex_voice->bit_resolution = scale(args[0], 16, 4, 1);
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::yes, limit::clamp> attack { this, "attack", 1, range { 1, 1000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                float attack = args[0];
                float slope = 0.75;
                float attack1_rate = attack * (1 - slope);
                float attack2_rate = attack * slope;

                triplex_voice->op1.envelope.attack1_rate = attack1_rate;
                triplex_voice->op1.envelope.attack1_level = slope;
                triplex_voice->op1.envelope.attack2_rate = attack2_rate;
                triplex_voice->op2.envelope.attack1_rate = attack1_rate;
                triplex_voice->op2.envelope.attack1_level = slope;
                triplex_voice->op2.envelope.attack2_rate = attack2_rate;
                triplex_voice->op3.envelope.attack1_rate = attack1_rate;
                triplex_voice->op3.envelope.attack1_level = slope;
                triplex_voice->op3.envelope.attack2_rate = attack2_rate;
            }
            return args;
        }}
    };

    attribute<number, threadsafe::yes, limit::clamp> decay { this, "decay", 100, range { 50, 1000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                float decay = args[0];
                float slope = 0.25;
                float decay1_rate = decay * slope;
                float decay2_rate = decay * (1 - slope);

                triplex_voice->op1.envelope.decay1_rate = decay1_rate;
                triplex_voice->op1.envelope.decay2_rate = decay2_rate;
                triplex_voice->op1.envelope.release1_rate = decay1_rate;
                triplex_voice->op1.envelope.release2_rate = decay2_rate;
                triplex_voice->op2.envelope.decay1_rate = decay1_rate;
                triplex_voice->op2.envelope.decay2_rate = decay2_rate;
                triplex_voice->op2.envelope.release1_rate = decay1_rate;
                triplex_voice->op2.envelope.release2_rate = decay2_rate;
                triplex_voice->op3.envelope.decay1_rate = decay1_rate;
                triplex_voice->op3.envelope.decay2_rate = decay2_rate;
                triplex_voice->op3.envelope.release1_rate = decay1_rate;
                triplex_voice->op3.envelope.release2_rate = decay2_rate;
            }
            return args;
        }}
    };

    attribute<number, threadsafe::yes, limit::clamp> sustain { this, "sustain", 0.25, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                float sustain = args[0];
                float slope = 0.25;
                float decay_level = sustain + ((1 - sustain) * slope);
                float release_level = sustain * slope;

                triplex_voice->op1.envelope.sustain_level = sustain;
                triplex_voice->op1.envelope.decay1_level = decay_level;
                triplex_voice->op1.envelope.release1_level = release_level;
                triplex_voice->op2.envelope.sustain_level = sustain;
                triplex_voice->op2.envelope.decay1_level = decay_level;
                triplex_voice->op2.envelope.release1_level = release_level;
                triplex_voice->op3.envelope.sustain_level = sustain;
                triplex_voice->op3.envelope.decay1_level = decay_level;
                triplex_voice->op3.envelope.release1_level = release_level;
            }
            return args;
        }}
    };

	void operator()(audio_bundle input, audio_bundle output) {
		long frames = input.frame_count();
		double* out1 = output.samples(0);

        while (--frames >= 0)
	    {    
            *out1 = triplex_voice->process_sample();
			*out1++;
        }
    }

private:
    std::unique_ptr<tx_voice> triplex_voice { nullptr };

    float scale(float input, float rangeMin, float rangeMax, float exponent)
    {
        return powf(input, exponent) * (rangeMax - rangeMin) + rangeMin;
    }
};

MIN_EXTERNAL(txc);
