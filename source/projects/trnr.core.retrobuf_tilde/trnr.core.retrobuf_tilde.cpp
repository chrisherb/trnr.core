#include "c74_min.h"
#include "../../trnr-lib/chebyshev.h"
#include "../../trnr-lib/ulaw.h"
#include "../../trnr-lib/tx_envelope.h"

using namespace c74::min;
using namespace trnr::core::lib;

class retrobuf : public object<retrobuf>, public vector_operator<> {
public:
    MIN_DESCRIPTION	{ "Read from a buffer~ like it's 1995" };
    MIN_TAGS		{ "audio, sampling" };
    MIN_AUTHOR		{ "Christopher Herb" };

    inlet<>  in1 { this, "(signal) Pitch, (bang) trigger, (int) gate on/off, (list) envelope values" };
    inlet<>  in2 { this, "(signal) Samplerate" };
    inlet<>  in3 { this, "(signal) Bitrate" };
    inlet<>  in4 { this, "(signal) Jitter" };
	outlet<> out1 {this, "(signal) Output1", "signal"};
	outlet<> out2 {this, "(signal) Output2", "signal"};
    outlet<> out3 {this, "(list) Amp envelope coordinates", "list"};

    message<> dspsetup {this, "dspsetup",
        MIN_FUNCTION {
           playback_pos = -1;
           buffer_size = 0;
           auto sr = samplerate();
           filter1.set_samplerate(sr);
           filter2.set_samplerate(sr);
           amp_env.set_samplerate(sr);
           return {};
       }
    };

    message<> bang { this, "bang", "Trigger the sample/envelope",
        MIN_FUNCTION {
            if (sync) playback_pos = clamp(start, 0, buffer_size);
            trigger = true;
            return {};
        }
    };
    
    message<threadsafe::yes> integer { this, "int", "Gate on/off",
        MIN_FUNCTION {
            int arg = args[0];
            if (arg > 0) {
                gate = true;
            } else {
                gate = false;
            }
            return {};
        }
    };

    message<threadsafe::yes> list { this, "list", "Envelope values", 
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
                case 0: // amp envelope
                    amp_env.attack1_rate = attack1_rate;
                    amp_env.attack1_level = attack1_level;
                    amp_env.attack2_rate = attack2_rate;
                    amp_env.hold_rate = hold_rate;
                    amp_env.decay1_rate = decay1_rate;
                    amp_env.decay1_level = decay1_level;
                    amp_env.decay2_rate = decay2_rate;
                    amp_env.sustain_level = sustain_level;
                    amp_env.release1_rate = release1_rate;
                    amp_env.release1_level = release1_level;
                    amp_env.release2_rate = release2_rate;
                    out2.send(to_atoms(amp_env.calc_coordinates()));
                    break;
                case 1: // filter envelope
                    break;
            }
            return {};
        }
    };

    buffer_reference buffer { this };

    argument<symbol> buffername {this, "buffer-name", "Initial buffer~ from which to read.",
        MIN_ARGUMENT_FUNCTION {
            buffer.set(arg);
        }
    };
    
	attribute<number, threadsafe::no> midinote {this, "midinote", 48};
	attribute<bool, threadsafe::no> loop { this, "loop", false };
	attribute<bool, threadsafe::no> sync { this, "sync", true, 
        setter { MIN_FUNCTION {
            if (!args[0]) {
                playback_pos = clamp(start, 0, buffer_size);
            }
            return args;
        }}};
	attribute<number, threadsafe::no, limit::clamp> deviation {this, "deviation", 5, range {1, 10}};
	attribute<number, threadsafe::no> start {this, "start", 0, };
	attribute<number, threadsafe::no> end {this, "end", 0, };

    void operator()(audio_bundle input, audio_bundle output) {

        double* in1 = input.samples(0);
        double* in2 = input.samples(1);
        double* in3 = input.samples(2);
        double* in4 = input.samples(3);
        double* out1 = output.samples(0);
        double* out2 = output.samples(1);

        buffer_lock<> buf(buffer); // gain access to the buffer~ content
        buffer_size = buf.frame_count();
        size_t channel_count = buf.channel_count();
        double buffer_samplerate = buf.samplerate();
        double host_samplerate = samplerate();

        if (buf.valid()) {
            for (int i = 0; i < input.frame_count(); ++i) {
                double output_l = 0;
                double output_r = 0;
                double pitch = in1[i];
                double resamplerate = in2[i];
                double bitrate = in3[i];
                int jitter = in4[i];

                if (playback_pos >= clamp(end, start, buffer_size)) {
                    if (loop) {
                        playback_pos = clamp(start, 0, buffer_size);
                    } else {
                        playback_pos = -1;
                    }
                }
                    
                double note_ratio = midi_to_ratio(midinote + pitch);

                if (playback_pos > -1) {
                    double samplerate_divisor = host_samplerate / clamp(resamplerate, 44, 44100);
                    int quantized_index = static_cast<int>(static_cast<int>(playback_pos / samplerate_divisor) * samplerate_divisor);

                    playback_pos += note_ratio * (buffer_samplerate / host_samplerate);

                    output_l = buf.lookup(wrap(quantized_index + calc_jitter(jitter), buffer_size), 0);

                    // get second channel if there is any
                    if (channel_count > 0) {
                        output_r = buf.lookup(wrap(quantized_index + calc_jitter(jitter), buffer_size), 1);
                    } else {
                        output_r = output_l;
                    }

                    reduce_bitrate(output_l, output_r, bitrate);

                    // calculate imaging filter frequency + deviation
                    double filter_frequency = ((resamplerate / 2) * note_ratio) * deviation;

                    filter1.process_sample(output_l, filter_frequency);
                    filter2.process_sample(output_r, filter_frequency);
                }

                float amplitude = amp_env.process_sample(gate, trigger);
                trigger = false;

                out1[i] = output_l * amplitude;
                out2[i] = output_r * amplitude;
            }
        }
        else {
            output.clear();
        }
    }

private:
    double playback_pos;
    double buffer_size;
    bool trigger;
    bool gate;
    chebyshev filter1;
    chebyshev filter2;
    ulaw compander;
    tx_envelope amp_env {0};
    
    float midi_to_ratio(float midi_note) {
        return powf(powf(2, midi_note - 60.f), 1.f / 12.f);
    }

    void reduce_bitrate(double& value1, double& value2, double bit) {
        compander.encode_samples(value1, value2);

        float resolution = powf(2, bit);
        value1 = round(value1 * resolution) / resolution;
        value2 = round(value2 * resolution) / resolution;

        compander.decode_samples(value1, value2);
    }

    int calc_jitter(int jitter) {
        if (jitter > 0) {
            return static_cast<int>(rand() % jitter);
        } else {
            return 0;
        }
    }

    double clamp(double& value, double min, double max) {
        if (value < min) {
            value = min;
        } else if (value > max) {
            value = max;
        }
        return value;
    }

    double wrap(double value, double max) {
        while (value > max) {
            value =- max;
        }
        return value;
    }
};

MIN_EXTERNAL(retrobuf);
