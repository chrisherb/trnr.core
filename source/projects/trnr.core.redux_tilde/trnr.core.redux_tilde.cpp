#include "c74_min.h"

using namespace c74::min;

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

    	message<> dspsetup {this, "dspsetup",
		MIN_FUNCTION {
            fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
            fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
			
			return {};
		}
	};

	void operator()(audio_bundle _input, audio_bundle _output) {

        double sampleRate = samplerate();
		long sampleFrames = _input.frame_count();

        float resolution = powf(2, bit);
        
        double* in1 = _input.samples(0);
		double* in2 = _input.samples(1);
		double* out1 = _output.samples(0);
		double* out2 = _output.samples(1);

        while (--sampleFrames >= 0)
	    {
            double inputSampleL = *in1;
			double inputSampleR = *in2;

            // ulaw encoding
            static int noisesourceL = 0;
            static int noisesourceR = 850010;
            int residue;
            double applyresidue;
            
            noisesourceL = noisesourceL % 1700021; noisesourceL++;
            residue = noisesourceL * noisesourceL;
            residue = residue % 170003; residue *= residue;
            residue = residue % 17011; residue *= residue;
            residue = residue % 1709; residue *= residue;
            residue = residue % 173; residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleL += applyresidue;
            if (inputSampleL<1.2e-38 && -inputSampleL<1.2e-38) {
                inputSampleL -= applyresidue;
            }
            
            noisesourceR = noisesourceR % 1700021; noisesourceR++;
            residue = noisesourceR * noisesourceR;
            residue = residue % 170003; residue *= residue;
            residue = residue % 17011; residue *= residue;
            residue = residue % 1709; residue *= residue;
            residue = residue % 173; residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleR += applyresidue;
            if (inputSampleR<1.2e-38 && -inputSampleR<1.2e-38) {
                inputSampleR -= applyresidue;
            }
            
            if (inputSampleL > 1.0) inputSampleL = 1.0;
            if (inputSampleL < -1.0) inputSampleL = -1.0;
            
            if (inputSampleR > 1.0) inputSampleR = 1.0;
            if (inputSampleR < -1.0) inputSampleR = -1.0;
            
            if (inputSampleL > 0) inputSampleL = log(1.0+(255*fabs(inputSampleL))) / log(256);
            if (inputSampleL < 0) inputSampleL = -log(1.0+(255*fabs(inputSampleL))) / log(256);
            
            if (inputSampleR > 0) inputSampleR = log(1.0+(255*fabs(inputSampleR))) / log(256);
            if (inputSampleR < 0) inputSampleR = -log(1.0+(255*fabs(inputSampleR))) / log(256);

            // bit reduction
            inputSampleL = round(inputSampleL * resolution) / resolution;
			inputSampleR = round(inputSampleR * resolution) / resolution;

            // ulaw decoding
            if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
            if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
            
            if (inputSampleL > 1.0) inputSampleL = 1.0;
            if (inputSampleL < -1.0) inputSampleL = -1.0;
            
            if (inputSampleR > 1.0) inputSampleR = 1.0;
            if (inputSampleR < -1.0) inputSampleR = -1.0;
            
            if (inputSampleL > 0) inputSampleL = (pow(256,fabs(inputSampleL))-1.0) / 255;
            if (inputSampleL < 0) inputSampleL = -(pow(256,fabs(inputSampleL))-1.0) / 255;
            
            if (inputSampleR > 0) inputSampleR = (pow(256,fabs(inputSampleR))-1.0) / 255;
            if (inputSampleR < 0) inputSampleR = -(pow(256,fabs(inputSampleR))-1.0) / 255;

            // 64 bit stereo floating point dither
            fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
            fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
			
            *out1 = inputSampleL;
			*out2 = inputSampleR;

			*in1++;
			*in2++;
			*out1++;
			*out2++;
        }
    }

private:
    uint32_t fpdL;
	uint32_t fpdR;
};

MIN_EXTERNAL(redux);
