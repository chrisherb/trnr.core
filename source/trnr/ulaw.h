#include <stdlib.h>
#include <cstdint>
#include <cmath>

class ulaw {
public:
    ulaw() {
        fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
        fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
    }

    void encodeSamples(double& inputSampleL, double& inputSampleR) {

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
    }

    void decodeSamples(double& inputSampleL, double& inputSampleR) {

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
    }

private:
    uint32_t fpdL;
	uint32_t fpdR;
};