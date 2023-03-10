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

	inlet<> in1 {this, "(signal) FM, (bang) trigger, (int) gate, (list) pitch envelope values"};
    inlet<> in2 {this, "(list) OP1 envelope values", "list"};
    inlet<> in3 {this, "(list) OP2 envelope values", "list"};
    inlet<> in4 {this, "(list) OP3 envelope values", "list"};
	outlet<> out1 {this, "(signal) Output", "signal"};
	outlet<> out2 {this, "(list) Pitch envelope coordinates", "list"};
	outlet<> out3 {this, "(list) OP1 envelope coordinates", "list"};
	outlet<> out4 {this, "(list) OP2 envelope coordinates", "list"};
	outlet<> out5 {this, "(list) OP3 envelope coordinates", "list"};

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

    message<threadsafe::yes> lists { this, "list", "Envelope values", 
        MIN_FUNCTION {
            auto values = from_atoms<std::vector<double>>(args);
            if (values.size() < 7) {
                cerr << "that's not enough values! it should be exactly 7" << endl;
                return {};
            }

            float attack1_rate = values.at(0) * (1 - values.at(1));
            float attack1_level = values.at(1);
            float attack2_rate = values.at(0) * values.at(1);
            float hold_rate = 0.1;
            float decay1_rate = values.at(2) * values.at(3);
            float decay1_level = values.at(4) + ((1 - values.at(4)) * values.at(3));
            float decay2_rate = values.at(2) * (1 - values.at(3));
            float sustain_level = values.at(4);
            float release1_rate = values.at(5) * values.at(6);
            float release1_level = values.at(4) * values.at(6);
            float release2_rate = values.at(5) * (1 - values.at(6));

            switch(inlet) {
                case 0: // pitch envelope
                    triplex_voice->pitch_env.attack1_rate = attack1_rate;
                    triplex_voice->pitch_env.attack1_level = attack1_level;
                    triplex_voice->pitch_env.attack2_rate = attack2_rate;
                    triplex_voice->pitch_env.hold_rate = hold_rate;
                    triplex_voice->pitch_env.decay1_rate = decay1_rate;
                    triplex_voice->pitch_env.decay1_level = decay1_level;
                    triplex_voice->pitch_env.decay2_rate = decay2_rate;
                    triplex_voice->pitch_env.sustain_level = sustain_level;
                    triplex_voice->pitch_env.release1_rate = release1_rate;
                    triplex_voice->pitch_env.release1_level = release1_level;
                    triplex_voice->pitch_env.release2_rate = release2_rate;
                    out2.send(array_to_atoms(triplex_voice->pitch_env.calc_coordinates()));
                    break;
                case 1: // op1 envelope
                    triplex_voice->op1.envelope.attack1_rate = attack1_rate;
                    triplex_voice->op1.envelope.attack1_level = attack1_level;
                    triplex_voice->op1.envelope.attack2_rate = attack2_rate;
                    triplex_voice->op1.envelope.hold_rate = hold_rate;
                    triplex_voice->op1.envelope.decay1_rate = decay1_rate;
                    triplex_voice->op1.envelope.decay1_level = decay1_level;
                    triplex_voice->op1.envelope.decay2_rate = decay2_rate;
                    triplex_voice->op1.envelope.sustain_level = sustain_level;
                    triplex_voice->op1.envelope.release1_rate = release1_rate;
                    triplex_voice->op1.envelope.release1_level = release1_level;
                    triplex_voice->op1.envelope.release2_rate = release2_rate;
                    out3.send(array_to_atoms(triplex_voice->op1.envelope.calc_coordinates()));
                    break;
                case 2: // op2 envelope
                    triplex_voice->op2.envelope.attack1_rate = attack1_rate;
                    triplex_voice->op2.envelope.attack1_level = attack1_level;
                    triplex_voice->op2.envelope.attack2_rate = attack2_rate;
                    triplex_voice->op2.envelope.hold_rate = hold_rate;
                    triplex_voice->op2.envelope.decay1_rate = decay1_rate;
                    triplex_voice->op2.envelope.decay1_level = decay1_level;
                    triplex_voice->op2.envelope.decay2_rate = decay2_rate;
                    triplex_voice->op2.envelope.sustain_level = sustain_level;
                    triplex_voice->op2.envelope.release1_rate = release1_rate;
                    triplex_voice->op2.envelope.release1_level = release1_level;
                    triplex_voice->op2.envelope.release2_rate = release2_rate;
                    out4.send(array_to_atoms(triplex_voice->op2.envelope.calc_coordinates()));
                    break;
                case 3: // op3 envelope
                    triplex_voice->op3.envelope.attack1_rate = attack1_rate;
                    triplex_voice->op3.envelope.attack1_level = attack1_level;
                    triplex_voice->op3.envelope.attack2_rate = attack2_rate;
                    triplex_voice->op3.envelope.hold_rate = hold_rate;
                    triplex_voice->op3.envelope.decay1_rate = decay1_rate;
                    triplex_voice->op3.envelope.decay1_level = decay1_level;
                    triplex_voice->op3.envelope.decay2_rate = decay2_rate;
                    triplex_voice->op3.envelope.sustain_level = sustain_level;
                    triplex_voice->op3.envelope.release1_rate = release1_rate;
                    triplex_voice->op3.envelope.release1_level = release1_level;
                    triplex_voice->op3.envelope.release2_rate = release2_rate;
                    out5.send(array_to_atoms(triplex_voice->op3.envelope.calc_coordinates()));
                    break;
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

    atoms array_to_atoms(std::array<std::array<float, 2>, 9> coords) {
        atoms output;
        output.reserve(18);
        for (int i = 0; i < coords.size(); i++) {
            output.insert(output.end(), coords[i].begin(), coords[i].end());
        }
        return output;
    }
};

MIN_EXTERNAL(triplex);
