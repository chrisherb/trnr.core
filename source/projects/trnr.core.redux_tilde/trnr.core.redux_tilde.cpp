#include "c74_min.h"
#include "../../trnr-lib/ulaw.h"

using namespace c74::min;
using namespace trnr::core::lib;

class redux : public object<redux>, public vector_operator<> {
public:
    MIN_DESCRIPTION	{"Stereo bitrate reducer with ulaw compansion"};
    MIN_TAGS		{"lofi"};
    MIN_AUTHOR		{"Christopher Herb"};

	inlet<> in1 {this, "(signal) Input1"};
	inlet<> in2 {this, "(signal) Input2"};
	outlet<> out1 {this, "(signal) Output1", "signal"};
	outlet<> out2 {this, "(signal) Output2", "signal"};

	attribute<number, threadsafe::no, limit::clamp> bit {this, "bit", 8, range {2, 16}};

	void operator()(audio_bundle _input, audio_bundle _output) {

        double sampleRate = samplerate();
		long sampleFrames = _input.frame_count();
        
        double* in1 = _input.samples(0);
		double* in2 = _input.samples(1);
		double* out1 = _output.samples(0);
		double* out2 = _output.samples(1);

        while (--sampleFrames >= 0)
	    {    
            double inputSampleL = *in1;
			double inputSampleR = *in2;

            // ulaw encoding
            compander.encode_samples(inputSampleL, inputSampleR);

            // bit reduction
            float resolution = powf(2, bit);
            inputSampleL = round(inputSampleL * resolution) / resolution;
			inputSampleR = round(inputSampleR * resolution) / resolution;

            // ulaw decoding
            compander.decode_samples(inputSampleL, inputSampleR);
			
            *out1 = inputSampleL;
			*out2 = inputSampleR;

			*in1++;
			*in2++;
			*out1++;
			*out2++;
        }
    }

private:
    ulaw compander;
};

MIN_EXTERNAL(redux);
