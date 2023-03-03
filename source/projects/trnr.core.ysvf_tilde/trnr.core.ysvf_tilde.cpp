#include "c74_min.h"
#include "ylowpass.h"
#include "yhighpass.h"
#include "ybandpass.h"
#include "ynotch.h"

using namespace c74::min;

class ysvf : public object<ysvf>, public vector_operator<> {
public:
    MIN_DESCRIPTION	{"State variable filter based on the Y filters by Chris Johnson: soft and smooth to nasty, edgy texture-varying filtering"};
    MIN_TAGS		{"filter"};
    MIN_AUTHOR		{"Christopher Herb"};

	inlet<> in1 {this, "(signal) Input1"};
	inlet<> in2 {this, "(signal) Input2"};
	inlet<> in3 {this, "(signal) Frequency"};
	inlet<> in4 {this, "(signal) Resonance"};
	inlet<> in5 {this, "(signal) Gain"};
	outlet<> out1 {this, "(signal) Output1", "signal"};
	outlet<> out2 {this, "(signal) Output2", "signal"};

	attribute<number, threadsafe::no, limit::clamp> edge {this, "edge", 0.1, range {0.0, 1.0}, 
        setter { MIN_FUNCTION { 
            lowpass.setEdge(args[0]);
            highpass.setEdge(args[0]);
            bandpass.setEdge(args[0]);
            notch.setEdge(args[0]);
            return args;
        }}
    };
	attribute<number, threadsafe::no, limit::clamp> output {this, "output", 1.0, range {0.0, 1.0}, 
        setter { MIN_FUNCTION { 
            lowpass.setOutput(args[0]);
            highpass.setOutput(args[0]);
            bandpass.setOutput(args[0]);
            notch.setOutput(args[0]);
            return args;
        }}
    };
	attribute<number, threadsafe::no, limit::clamp> mix {this, "mix", 1.0, range {0.0, 1.0}, 
        setter { MIN_FUNCTION { 
            lowpass.setMix(args[0]);
            highpass.setMix(args[0]);
            bandpass.setMix(args[0]);
            notch.setMix(args[0]);
            return args;
        }}
    };
	attribute<int, threadsafe::no, limit::clamp> filtertype {this, "filtertype", 0, range {0, 3}, 
        description { "0 = lowpass, 1 = highpass, 2 = bandpass, 3 = notch" } 
    };

	void operator()(audio_bundle input, audio_bundle output) {

        double sampleRate = samplerate();
		long sampleFrames = input.frame_count();

        double freq = clamp(input.samples(2)[0], 0, 1);
        lowpass.setFrequency(freq);
        highpass.setFrequency(freq);
        bandpass.setFrequency(freq);
        notch.setFrequency(freq);

        double res = clamp(input.samples(3)[0], 0, 1);
        lowpass.setResonance(res);
        highpass.setResonance(res);
        bandpass.setResonance(res);
        notch.setResonance(res);

        double gain = clamp(input.samples(4)[0], 0, 1);
        lowpass.setDrive(gain);
        highpass.setDrive(gain);
        bandpass.setDrive(gain);
        notch.setDrive(gain);
        
        switch(filtertype){
            case 0:
                lowpass.processblock(input.samples(), output.samples(), sampleRate, sampleFrames);
                break;
            case 1:
                highpass.processblock(input.samples(), output.samples(), sampleRate, sampleFrames);
                break;
            case 2:
                bandpass.processblock(input.samples(), output.samples(), sampleRate, sampleFrames);
                break;
            case 3:
                notch.processblock(input.samples(), output.samples(), sampleRate, sampleFrames);
                break;
        }
    }

private:
    ylowpass lowpass;
    yhighpass highpass;
    ybandpass bandpass;
    ynotch notch;

    double clamp(double& value, double min, double max) {
        if (value < min) {
            value = min;
        } else if (value > max) {
            value = max;
        }
        return value;
    }
};

MIN_EXTERNAL(ysvf);
