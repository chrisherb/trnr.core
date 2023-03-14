#include "c74_min.h"
#include "../../trnr-lib/ysvf.h"

using namespace c74::min;
using namespace trnr::core::lib;

class svf : public object<svf>, public vector_operator<> {
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
            state_variable_filter.set_edge(args[0]);
            return args;
        }}
    };
	attribute<number, threadsafe::no, limit::clamp> output {this, "output", 1.0, range {0.0, 1.0}, 
        setter { MIN_FUNCTION { 
            state_variable_filter.set_output(args[0]);
            return args;
        }}
    };
	attribute<number, threadsafe::no, limit::clamp> mix {this, "mix", 1.0, range {0.0, 1.0}, 
        setter { MIN_FUNCTION {
            state_variable_filter.set_mix(args[0]);
            return args;
        }}
    };
	attribute<int, threadsafe::no, limit::clamp> filtertype {this, "filtertype", 0, range {0, 3}, 
        description { "0 = lowpass, 1 = highpass, 2 = bandpass, 3 = notch" }, 
        setter { MIN_FUNCTION {
            state_variable_filter.set_filter_type(args[0]);
            return args;
        }}
    };

	void operator()(audio_bundle input, audio_bundle output) {

		long sample_frames = input.frame_count();

        double freq = clamp(input.samples(2)[0], 0, 1);
        state_variable_filter.set_frequency(freq);

        double res = clamp(input.samples(3)[0], 0, 1);
        state_variable_filter.set_resonance(res);

        double gain = clamp(input.samples(4)[0], 0, 1);
        state_variable_filter.set_drive(gain);

        state_variable_filter.process_block(input.samples(), output.samples(), sample_frames);
    }

private:
    ysvf state_variable_filter;

    double clamp(double& value, double min, double max) {
        if (value < min) {
            value = min;
        } else if (value > max) {
            value = max;
        }
        return value;
    }
};

MIN_EXTERNAL(svf);
