#pragma once
#include <cmath>

namespace trnr::core::lib {

class tx_sineosc {
public:
    float phase_resolution;
    bool phase_reset;

    tx_sineosc(double _samplerate) 
        : samplerate { _samplerate }
        , phase_resolution { 16.f }
        , phase { 0. }
        , history { 0. }
        , phase_reset { false }
    {
    }

    float process_sample(bool trigger, float frequency, float phase_modulation = 0.f) {
        if (trigger && phase_reset) {
            phase = 0.0;
        }

        float lookup_phase = phase + phase_modulation;
        wrap(lookup_phase);
        phase += frequency / samplerate;
        wrap(phase);

        redux(lookup_phase, phase_resolution);

        float output = sine(lookup_phase * 4096.);

        filter(output);
        return output;
    }

    void setSampleRate(double _samplerate) {
        this->samplerate = _samplerate;
    }

private:
    double samplerate;
    float phase;
    float history;

    float sine(float x) {
        // x is scaled 0<=x<4096
        const float a = -0.40319426317E-08;
        const float b = 0.21683205691E+03;
        const float c = 0.28463350538E-04;
        const float d = -0.30774648337E-02;
        float y;

        bool negate = false;
        if (x > 2048) {
            negate = true;
            x -= 2048;
        }
        if (x > 1024)
            x = 2048 - x;
        y = (a + x) / (b + c * x * x) + d * x;
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