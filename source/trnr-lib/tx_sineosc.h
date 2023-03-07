#pragma once
#include <cmath>

namespace trnr::core::lib {

class tx_sineosc {
public:
    float phaseResolution;
    bool phaseReset;

    tx_sineosc(double sampleRate) 
        : sampleRate { sampleRate }
        , phaseResolution { 16.f }
        , phase { 0. }
        , history { 0. }
        , phaseReset { false }
    {
    }

    float processSample(bool trigger, float frequency, float phaseModulation = 0.f) {
        if (trigger && phaseReset) {
            phase = 0.0;
        }

        float lookupPhase = phase + phaseModulation;
        wrap(lookupPhase);
        phase += frequency / sampleRate;
        wrap(phase);

        redux(lookupPhase, phaseResolution);

        float output = sine(lookupPhase * 4096.);

        filter(output);
        return output;
    }

    void setSampleRate(double sampleRate) {
        this->sampleRate = sampleRate;
    }

private:
    double sampleRate;
    float phase;
    float history;

    float sine(float x) {
        // x is scaled 0<=x<4096
        const float A = -0.40319426317E-08;
        const float B = 0.21683205691E+03;
        const float C = 0.28463350538E-04;
        const float D = -0.30774648337E-02;
        float y;

        bool negate = false;
        if (x > 2048) {
            negate = true;
            x -= 2048;
        }
        if (x > 1024)
            x = 2048 - x;
        y = (A + x) / (B + C * x * x) + D * x;
        if (negate)
            return (float)(-y);
        else
            return (float)y;
    }

    float wrap(float& phase) {
        while (phase < 0.)
            phase += 1.;

        while (phase >= 1.)
            phase -= 1.;

        return phase;
    }

    float filter(float& value) {
        value = 0.5 * (value + history);
        history = value;
        return value;
    }

    float redux(float& value, float resolution)
    {
        float res = powf(2, resolution);
        value = roundf(value * res) / res;

        return value;
    }
};
}