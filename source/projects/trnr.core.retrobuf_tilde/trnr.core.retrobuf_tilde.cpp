#include "c74_min.h"
#include "../../trnr-lib/chebyshev.h"
#include "../../trnr-lib/ulaw.h"

using namespace c74::min;
using namespace trnr::core::lib;

class retrobuf : public object<retrobuf>, public vector_operator<> {
public:
    MIN_DESCRIPTION	{ "Read from a buffer~ like it's 1995" };
    MIN_TAGS		{ "audio, sampling" };
    MIN_AUTHOR		{ "Christopher Herb" };

    inlet<>  in1 { this, "(signal) Pitch" };
    inlet<>  in2 { this, "(signal) Samplerate" };
    inlet<>  in3 { this, "(signal) Bitrate" };
    inlet<>  in4 { this, "(signal) Jitter" };
	outlet<> out1 {this, "(signal) Output1", "signal"};
	outlet<> out2 {this, "(signal) Output2", "signal"};

    message<> dspsetup {this, "dspsetup",
        MIN_FUNCTION {
           playback_pos = -1;
           buffer_size = 0;
           auto sr = samplerate();
           filter1.set_samplerate(sr);
           filter2.set_samplerate(sr);
           return {};
       }
    };

    message<> trigger { this, "bang", "Trigger the sample",
        MIN_FUNCTION {
            if (sync) playback_pos = clamp(start, 0, buffer_size);
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
                    
                double noteRatio = midi_to_ratio(midinote + pitch);

                if (playback_pos > -1) {
                    double samplerate_divisor = host_samplerate / clamp(resamplerate, 44, 44100);
                    int quantized_index = static_cast<int>(static_cast<int>(playback_pos / samplerate_divisor) * samplerate_divisor);

                    playback_pos += noteRatio * (buffer_samplerate / host_samplerate);

                    output_l = buf.lookup(wrap(quantized_index + calc_jitter(jitter), buffer_size), 0);

                    // get second channel if there is any
                    if (channel_count > 0) {
                        output_r = buf.lookup(wrap(quantized_index + calc_jitter(jitter), buffer_size), 1);
                    } else {
                        output_r = output_l;
                    }

                    reduce_bitrate(output_l, output_r, bitrate);

                    // calculate imaging filter frequency + deviation
                    double filterFrequency = ((resamplerate / 2) * noteRatio) * deviation;

                    filter1.process_sample(output_l, filterFrequency);
                    filter2.process_sample(output_r, filterFrequency);
                }

                out1[i] = output_l;
                out2[i] = output_r;
            }
        }
        else {
            output.clear();
        }
    }

private:
    double playback_pos;
    double buffer_size;
    chebyshev filter1;
    chebyshev filter2;
    ulaw compander;

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
