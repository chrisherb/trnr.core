#include "c74_min.h"
#include "../../trnr-lib/tx_voice.h"

using namespace c74::min;
using namespace trnr::core::lib;

class triplex : public object<triplex>, public vector_operator<> {
private:
    bool initialized;
public:
    MIN_DESCRIPTION	{"3 operator FM synth voice"};
    MIN_TAGS		{"synth voice"};
    MIN_AUTHOR		{"Christopher Herb"};

	inlet<> in1 {this, "(signal) FM"};
	outlet<> out1 {this, "(signal) Output", "signal"};

    message<> setup {this, "setup",
        MIN_FUNCTION {
            triplex_voice = std::make_unique<tx_voice>(0);
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

    message<> trigger { this, "bang", "Trigger",
        MIN_FUNCTION {
            triplex_voice->trigger = true;
            return {};
        }
    };

    message<> gate { this, "int", "Gate on/off",
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

    attribute<number, threadsafe::no, limit::clamp> velocity { this, "velocity", 100, range { 0, 127 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                double vel = args[0];
                triplex_voice->velocity = vel / 127.0;
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> frequency { this, "frequency", 100, range { 20, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->frequency = args[0];
            }
            return args;
        }}
    };
    
    attribute<int, threadsafe::no, limit::clamp> algorithm { this, "algorithm", 0, range { 0, 3 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->algorithm = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> pitch_env_amt { this, "pitch_env_amt", 0, range { 0, 300 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->pitch_env_amt = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> feedback { this, "feedback", 0, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->feedback_amt = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> phase_resolution { this, "phase_resolution", 12, range { 2, 16 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.oscillator.phase_resolution = args[0];
                triplex_voice->op2.oscillator.phase_resolution = args[0];
                triplex_voice->op3.oscillator.phase_resolution = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> bit_resolution { this, "bit_resolution", 12, range { 2, 16 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->bit_resolution = args[0];
            }
            return args;
        }}
    };
    
    // PITCH ENVELOPE

    attribute<number, threadsafe::no, limit::clamp> pitch_attack_rate { this, "pitch_attack_rate", 10, range { 1, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->pitch_env.attack_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> pitch_attack_curvature { this, "pitch_attack_curvature", 0.5, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->pitch_env.attack_curvature = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> pitch_hold_rate { this, "pitch_hold_rate", 1, range { 1, 10000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->pitch_env.hold_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> pitch_decay_rate { this, "pitch_decay_rate", 1, range { 1, 10000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->pitch_env.decay_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> pitch_decay_level { this, "pitch_decay_level", 0.75, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->pitch_env.decay_level = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> pitch_sustain_level { this, "pitch_sustain_level", 0.5, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->pitch_env.sustain_level = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> pitch_release_rate { this, "pitch_release_rate", 100, range { 1, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->pitch_env.release_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> pitch_release_curvature { this, "pitch_release_curvature", 0.25, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->pitch_env.release_curvature = args[0];
            }
            return args;
        }}
    };

    // OP 1
    
    attribute<number, threadsafe::no, limit::clamp> op1_ratio { this, "op1_ratio", 1, range { 0.1, 48 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.ratio = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op1_amplitude { this, "op1_amplitude", 1, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.amplitude = args[0];
            }
            return args;
        }}
    };

    attribute<number, threadsafe::no, limit::clamp> op1_attack_rate { this, "op1_attack_rate", 10, range { 1, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.envelope.attack_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op1_attack_curvature { this, "op1_attack_curvature", 0.5, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.envelope.attack_curvature = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op1_hold_rate { this, "op1_hold_rate", 1, range { 1, 10000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.envelope.hold_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op1_decay_rate { this, "op1_decay_rate", 1, range { 1, 10000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.envelope.decay_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op1_decay_level { this, "op1_decay_level", 0.75, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.envelope.decay_level = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op1_sustain_level { this, "op1_sustain_level", 0.5, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.envelope.sustain_level = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op1_release_rate { this, "op1_release_rate", 100, range { 1, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.envelope.release_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op1_release_curvature { this, "op1_release_curvature", 0.25, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op1.envelope.release_curvature = args[0];
            }
            return args;
        }}
    };

    // // OP 2
    
    attribute<number, threadsafe::no, limit::clamp> op2_ratio { this, "op2_ratio", 1, range { 0.1, 48 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.ratio = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op2_amplitude { this, "op2_amplitude", 1, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.amplitude = args[0];
            }
            return args;
        }}
    };

    attribute<number, threadsafe::no, limit::clamp> op2_attack_rate { this, "op2_attack_rate", 10, range { 1, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.envelope.attack_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op2_attack_curvature { this, "op2_attack_curvature", 0.5, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.envelope.attack_curvature = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op2_hold_rate { this, "op2_hold_rate", 1, range { 1, 10000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.envelope.hold_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op2_decay_rate { this, "op2_decay_rate", 1, range { 1, 10000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.envelope.decay_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op2_decay_level { this, "op2_decay_level", 0.75, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.envelope.decay_level = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op2_sustain_level { this, "op2_sustain_level", 0.5, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.envelope.sustain_level = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op2_release_rate { this, "op2_release_rate", 100, range { 1, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.envelope.release_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op2_release_curvature { this, "op2_release_curvature", 0.25, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op2.envelope.release_curvature = args[0];
            }
            return args;
        }}
    };

    // // OP 3
    
    attribute<number, threadsafe::no, limit::clamp> op3_ratio { this, "op3_ratio", 1, range { 0.1, 48 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.ratio = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op3_amplitude { this, "op3_amplitude", 1, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.amplitude = args[0];
            }
            return args;
        }}
    };

    attribute<number, threadsafe::no, limit::clamp> op3_attack_rate { this, "op3_attack_rate", 10, range { 1, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.envelope.attack_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op3_attack_curvature { this, "op3_attack_curvature", 0.5, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.envelope.attack_curvature = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op3_hold_rate { this, "op3_hold_rate", 1, range { 1, 10000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.envelope.hold_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op3_decay_rate { this, "op3_decay_rate", 1, range { 1, 10000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.envelope.decay_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op3_decay_level { this, "op3_decay_level", 0.75, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.envelope.decay_level = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op3_sustain_level { this, "op3_sustain_level", 0.5, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.envelope.sustain_level = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op3_release_rate { this, "op3_release_rate", 100, range { 1, 20000 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.envelope.release_rate = args[0];
            }
            return args;
        }}
    };
    
    attribute<number, threadsafe::no, limit::clamp> op3_release_curvature { this, "op3_release_curvature", 0.25, range { 0, 1 }, 
        setter { MIN_FUNCTION {
            if (initialized) {
                triplex_voice->op3.envelope.release_curvature = args[0];
            }
            return args;
        }}
    };

	void operator()(audio_bundle input, audio_bundle output) {
		long frames = input.frame_count();
        
        double* in1 = input.samples(0);
		double* out1 = output.samples(0);

        while (--frames >= 0)
	    {    
            float frequency = *in1;

            // triplex_voice->frequency += frequency;

            *out1 = triplex_voice->process_sample();

			*in1++;
			*out1++;
        }
    }

private:
    std::unique_ptr<tx_voice> triplex_voice { nullptr };
};

MIN_EXTERNAL(triplex);
