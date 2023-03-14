#include "ylowpass.h"
#include "yhighpass.h"
#include "ybandpass.h"
#include "ynotch.h"

namespace trnr::core::lib {

enum filter_types {
    highpass = 0,
    lowpass,
    bandpass,
    notch
};

class ysvf {
public:
    ysvf(double _samplerate)
        : lowpass { _samplerate }
        , highpass { _samplerate }
        , bandpass { _samplerate }
        , notch { _samplerate }
    {}

    void set_samplerate(double _samplerate) {
        lowpass.set_samplerate(_samplerate);
        highpass.set_samplerate(_samplerate);
        bandpass.set_samplerate(_samplerate);
        notch.set_samplerate(_samplerate);
    }

    void set_filter_type(filter_types type) {
        filter_type = type;
    }

    void set_drive(float value) {
        lowpass.setDrive(value);
        highpass.setDrive(value);
        bandpass.setDrive(value);
        notch.setDrive(value);
    }

    void set_frequency(float value) {
        lowpass.setFrequency(value);
        highpass.setFrequency(value);
        bandpass.setFrequency(value);
        notch.setFrequency(value);
    }

    void set_resonance(float value) {
        lowpass.setResonance(value);
        highpass.setResonance(value);
        bandpass.setResonance(value);
        notch.setResonance(value);
    }

    void set_edge(float value) {
        lowpass.setEdge(value);
        highpass.setEdge(value);
        bandpass.setEdge(value);
        notch.setEdge(value);
    }

    void set_output(float value) {
        lowpass.setOutput(value);
        highpass.setOutput(value);
        bandpass.setOutput(value);
        notch.setOutput(value);
    }

    void set_mix(float value) {
        lowpass.setMix(value);
        highpass.setMix(value);
        bandpass.setMix(value);
        notch.setMix(value);
    }

    void process_block(double** inputs, double** outputs, int block_size) {

        switch (filter_type) {
            case filter_types::lowpass:
                lowpass.processblock(inputs, outputs, block_size);
                break;
            case filter_types::highpass:
                highpass.processblock(inputs, outputs, block_size);
                break;
            case filter_types::bandpass:
                bandpass.processblock(inputs, outputs, block_size);
                break;
            case filter_types::notch:
                notch.processblock(inputs, outputs, block_size);
                break;
        }
    }

private:
    filter_types filter_type;
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
}