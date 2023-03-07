#pragma once
#include "tx_sineosc.h"
#include "tx_envelope.h"

namespace trnr::core::lib {
class tx_operator {
public:
    tx_operator(double sampleRate)
        : ratio { 1 }
        , amplitude { 1.0f }
        , envelope(sampleRate)
        , oscillator(sampleRate)
    {
    }

    tx_envelope envelope;
    tx_sineosc oscillator;
    float ratio;
    float amplitude;

    float processSamplePM(const bool& gate, const bool& trigger, const float& frequency, const float& velocity, const float& pm = 0) {

        float env = envelope.processSample(gate, trigger);

        // drifts and sounds better!
        if (envelope.isBusy()) {
            double osc = oscillator.processSample(trigger, frequency, pm);
            return osc * env * velocity;
        } else {
            return 0.;
        }
    }

    void setSampleRate(double sampleRate) {
        this->envelope.setSampleRate(sampleRate);
        this->oscillator.setSampleRate(sampleRate);
    }
};
}
