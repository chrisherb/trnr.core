#pragma once
#include "tx_sineosc.h"
#include "tx_envelope.h"
#include "tx_operator.h"

namespace trnr::core::lib {

class tx_voice {
public:
    tx_voice(double sampleRate) 
        : algorithm { 0 }
        , pitchEnvelopeAmt { 0.f }
        , feedbackAmt { 0.f }
        , pitchEnvelope(sampleRate)
        , feedbackOsc(sampleRate)
        , op1(sampleRate)
        , op2(sampleRate)
        , op3(sampleRate)
    {
    }

    bool gate = false;
    bool trigger = false;
    float frequency = 100.f;
    float velocity = 1.f;

    int algorithm;
    float pitchEnvelopeAmt;
    float feedbackAmt;
    tx_sineosc feedbackOsc;
    tx_envelope pitchEnvelope;
    tx_operator op1;
    tx_operator op2;
    tx_operator op3;

    float processSample() {
        float pitchEnvelopeSignal = pitchEnvelope.processSample(gate, trigger) * pitchEnvelopeAmt;
        float pitchedFreq = frequency + pitchEnvelopeSignal;

        float output = 0.f;

        // mix operator signals according to selected algorithm
        switch (algorithm) {
        case 0:
            output = calcAlgo1(pitchedFreq);
            break;
        case 1:
            output = calcAlgo2(pitchedFreq);
            break;
        case 2:
            output = calcAlgo3(pitchedFreq);
            break;
        case 3:
            output = calcAlgo4(pitchedFreq);
            break;
        default:
            output = calcAlgo1(pitchedFreq);
            break;
        }

        // reset trigger
        trigger = false;

        return output;
    }

    bool isBusy() { return gate || op1.envelope.isBusy() || op2.envelope.isBusy() || op3.envelope.isBusy(); }

    void setSampleRate(double samplerate) {
        pitchEnvelope.setSampleRate(samplerate);
        feedbackOsc.setSampleRate(samplerate);
        op1.setSampleRate(samplerate);
        op2.setSampleRate(samplerate);
        op3.setSampleRate(samplerate);
    }

    void setPhaseReset(bool phaseReset) {
        op1.oscillator.phaseReset = phaseReset;
        op2.oscillator.phaseReset = phaseReset;
        op3.oscillator.phaseReset = phaseReset;
        feedbackOsc.phaseReset = phaseReset;
    }

private:
    const float MOD_INDEX_COEFF = 4.f;

    float calcAlgo1(const float frequency) {
        float feedbackFreq = frequency * op3.ratio;
        float feedbackModIndex = (feedbackAmt * MOD_INDEX_COEFF);
        float feedbackSignal = feedbackOsc.processSample(trigger, feedbackFreq) * feedbackModIndex;

        float op3Freq = frequency * op3.ratio;
        float op3ModIndex = (op3.amplitude * MOD_INDEX_COEFF);
        float op3Signal = op3.processSamplePM(gate, trigger, op3Freq, velocity, feedbackSignal) * op3ModIndex;

        float op2Freq = frequency * op2.ratio;
        float op2ModIndex = (op2.amplitude * MOD_INDEX_COEFF);
        float op2Signal = op2.processSamplePM(gate, trigger, op2Freq, velocity, op3Signal) * op2ModIndex;

        float op1Freq = frequency * op1.ratio;
        return op1.processSamplePM(gate, trigger, op1Freq, velocity, op2Signal) * op1.amplitude;
    }

    float calcAlgo2(const float frequency) {
        float feedbackFreq = frequency * op3.ratio;
        float feedbackModIndex = (feedbackAmt * MOD_INDEX_COEFF);
        float feedbackSignal = feedbackOsc.processSample(trigger, feedbackFreq) * feedbackModIndex;

        float op3Freq = frequency * op3.ratio;
        float op3Signal = op3.processSamplePM(gate, trigger, op3Freq, velocity, feedbackSignal) * op3.amplitude;

        float op2Freq = frequency * op2.ratio;
        float op2ModIndex = (op2.amplitude * MOD_INDEX_COEFF);
        float op2Signal = op2.processSamplePM(gate, trigger, op2Freq, velocity) * op2ModIndex;

        float op1Freq = frequency * op1.ratio;
        float op1Signal = op1.processSamplePM(gate, trigger, op1Freq, velocity, op2Signal) * op1.amplitude;

        return op1Signal + op3Signal;
    }

    float calcAlgo3(const float frequency) {
        float feedbackFreq = frequency * op3.ratio;
        float feedbackModIndex = (feedbackAmt * MOD_INDEX_COEFF);
        float feedbackSignal = feedbackOsc.processSample(trigger, feedbackFreq) * feedbackModIndex;

        float op3Freq = frequency * op3.ratio;
        float op3Signal = op3.processSamplePM(gate, trigger, op3Freq, velocity, feedbackSignal) * op3.amplitude;

        float op2Freq = frequency * op2.ratio;
        float op2Signal = op2.processSamplePM(gate, trigger, op2Freq, velocity) * op2.amplitude;

        float op1Freq = frequency * op1.ratio;
        float op1Signal = op1.processSamplePM(gate, trigger, op1Freq, velocity) * op1.amplitude;

        return op1Signal + op2Signal + op3Signal;
    }

    float calcAlgo4(const float frequency) {
        float feedbackFreq = frequency * op3.ratio;
        float feedbackModIndex = (feedbackAmt * MOD_INDEX_COEFF);
        float feedbackSignal = feedbackOsc.processSample(trigger, feedbackFreq) * feedbackModIndex;

        float op3Freq = frequency * op3.ratio;
        float op3ModIndex = (op3.amplitude * MOD_INDEX_COEFF);
        float op3Signal = op3.processSamplePM(gate, trigger, op3Freq, velocity, feedbackSignal) * op3ModIndex;

        float op2Freq = frequency * op2.ratio;
        float op2ModIndex = (op2.amplitude * MOD_INDEX_COEFF);
        float op2Signal = op2.processSamplePM(gate, trigger, op2Freq, velocity) * op2ModIndex;

        float op1Freq = frequency * op1.ratio;
        return op1.processSamplePM(gate, trigger, op1Freq, velocity, op2Signal + op3Signal) * op1.amplitude;
    }
};
}