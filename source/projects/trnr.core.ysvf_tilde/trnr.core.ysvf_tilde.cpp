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
	outlet<> out1 {this, "(signal) Output1", "signal"};
	outlet<> out2 {this, "(signal) Output2", "signal"};

	attribute<number, threadsafe::no, limit::clamp> gain { this, "gain", 0.1, range {0.0, 1.0}, 
        setter { MIN_FUNCTION { 
            lowpass.setDrive(args[0]);
            highpass.setDrive(args[0]);
            bandpass.setDrive(args[0]);
            notch.setDrive(args[0]);
            return args;
        }}
    };
	attribute<number, threadsafe::no, limit::clamp> resonance {this, "resonance", 0.1, range {0.0, 1.0}, 
        setter { MIN_FUNCTION { 
            lowpass.setResonance(args[0]);
            highpass.setResonance(args[0]);
            bandpass.setResonance(args[0]);
            notch.setResonance(args[0]);
            return args;
        }}
    };
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

	void operator()(audio_bundle _input, audio_bundle _output) {

        double sampleRate = samplerate();
		long sampleFrames = _input.frame_count();

        double freq = _input.samples(2)[0];
        if (freq > 1.0)
            freq = 1.0;
        else if (freq < 0.0)
            freq = 0.0;

        lowpass.setFrequency(freq);
        highpass.setFrequency(freq);
        bandpass.setFrequency(freq);
        notch.setFrequency(freq);
        
        switch(filtertype){
            case 0:
                lowpass.processblock(_input.samples(), _output.samples(), sampleRate, sampleFrames);
                break;
            case 1:
                highpass.processblock(_input.samples(), _output.samples(), sampleRate, sampleFrames);
                break;
            case 2:
                bandpass.processblock(_input.samples(), _output.samples(), sampleRate, sampleFrames);
                break;
            case 3:
                notch.processblock(_input.samples(), _output.samples(), sampleRate, sampleFrames);
                break;
        }
    }

private:
    ylowpass lowpass;
    yhighpass highpass;
    ybandpass bandpass;
    ynotch notch;
};

MIN_EXTERNAL(ysvf);
