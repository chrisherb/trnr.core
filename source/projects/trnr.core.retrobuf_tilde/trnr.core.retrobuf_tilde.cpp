#include "c74_min.h"
#include "../../trnr/chebyshev.h"

using namespace c74::min;
using namespace c74::min::lib::math;

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
           playbackPos = -1;
           return {};
       }
    };

    message<> trigger { this, "bang", "Trigger the sample",
        MIN_FUNCTION {
            if (!looping) playbackPos = 0;
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
	attribute<int, threadsafe::no, limit::clamp> looping {this, "loop", 0, range {0, 1}};
	attribute<number, threadsafe::no, limit::clamp> velocity {this, "velocity", 100, range {0, 127}};
	attribute<number, threadsafe::no, limit::clamp> deviation {this, "deviation", 5, range {1, 10}};
	attribute<number, threadsafe::no> start {this, "start", 0}; // TODO
	attribute<number, threadsafe::no> end {this, "end", 0}; // TODO

    void operator()(audio_bundle input, audio_bundle output) {

        double* in1 = input.samples(0);
        double* in2 = input.samples(1);
        double* in3 = input.samples(2);
        double* in4 = input.samples(3);
        double* out1 = output.samples(0);
        double* out2 = output.samples(1);

        buffer_lock<> buf(buffer); // gain access to the buffer~ content
        size_t channelCount = buf.channel_count();
        double bufferSamplerate = buf.samplerate();
        double hostSamplerate = samplerate();

        if (buf.valid()) {
            for (int i = 0; i < input.frame_count(); ++i) {

                double outputL = 0;
                double outputR = 0;
                double pitch = in1[i];
                double resamplerate = in2[i];
                double bitrate = in3[i];
                double jitter = in4[i];

                if (playbackPos >= buf.frame_count()) {
                    if (looping) {
                        playbackPos = 0;
                    } else {
                        playbackPos = -1;
                    }
                }
                    
                double noteRatio = midiToRatio(midinote) + pitch;

                if (playbackPos > -1) {
                    double samplerateDivisor = hostSamplerate / clamp(resamplerate, 44, 44100);
                    int quantizedIndex = floor(floor(playbackPos / samplerateDivisor) * samplerateDivisor);

                    playbackPos += noteRatio * (bufferSamplerate / hostSamplerate);

                    double noise1 = random(0, 1) * jitter;
                    double frameCount = buf.frame_count();
                    outputL = buf.lookup(wrap(quantizedIndex + noise1, frameCount), 0);

                    // get second channel if there is any
                    if (channelCount > 0) {
                        double noise2 = random(0, 1) * jitter;
                        outputR = buf.lookup(wrap(quantizedIndex + noise2, frameCount), 1);
                    } else {
                        outputR = outputL;
                    }
                }

                // calculate imaging filter frequency + deviation
                double filterFrequency = ((resamplerate / 2) * noteRatio) * deviation;
                double vel = velocity / 127;

                // TODO: bitrate reduction

                out1[i] = filter1.processSample(outputL, filterFrequency, hostSamplerate) * vel;
                out2[i] = filter2.processSample(outputR, filterFrequency, hostSamplerate) * vel;
            }
        }
        else {
            output.clear();
        }
    }

private:
    double playbackPos;
    chebyshev filter1;
    chebyshev filter2;

    float midiToRatio(float midiNote)
    {
        return powf(powf(2, midiNote - 60.f), 1.f / 12.f);
    }

    double clamp(double value, double min, double max) {
        
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
