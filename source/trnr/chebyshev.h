#include <array>

class chebyshev {
public:
    void processSample(double& input, double frequency, double samplerate) {

        if (frequency >= 20000.f) {
            frequency = 20000.f;
        }

        // First calculate the prewarped digital frequency :
        auto K = tanf(M_PI * frequency / samplerate);

        // Now we calc some Coefficients :
        auto sg = sinh(passbandRipple);
        auto cg = cosh(passbandRipple);
        cg *= cg;

        std::array<double, 4> coeff;
        coeff[0] = 1 / (cg - 0.85355339059327376220042218105097);
        coeff[1] = K * coeff[0] * sg * 1.847759065022573512256366378792;
        coeff[2] = 1 / (cg - 0.14644660940672623779957781894758);
        coeff[3] = K * coeff[2] * sg * 0.76536686473017954345691996806;

        K *= K; // (just to optimize it a little bit)

        // Calculate the first biquad:
        A0 = 1 / (coeff[1] + K + coeff[0]);
        A1 = 2 * (coeff[0] - K) * A0;
        A2 = (coeff[1] - K - coeff[0]) * A0;
        B0 = A0 * K;
        B1 = 2 * B0;
        B2 = B0;

        // Calculate the second biquad:
        A3 = 1 / (coeff[3] + K + coeff[2]);
        A4 = 2 * (coeff[2] - K) * A3;
        A5 = (coeff[3] - K - coeff[2]) * A3;
        B3 = A3 * K;
        B4 = 2 * B3;
        B5 = B3;

        // Then calculate the output as follows:
        auto Stage1 = B0 * input + State0;
        State0 = B1 * input + A1 * Stage1 + State1;
        State1 = B2 * input + A2 * Stage1;
        input = B3 * Stage1 + State2;
        State2 = B4 * Stage1 + A4 * input + State3;
        State3 = B5 * Stage1 + A5 * input;
    }

private:
    double A0 = 0;
    double A1 = 0;
    double A2 = 0;
    double A3 = 0;
    double A4 = 0;
    double A5 = 0;
    double B0 = 0;
    double B1 = 0;
    double B2 = 0;
    double B3 = 0;
    double B4 = 0;
    double B5 = 0;
    double State0 = 0;
    double State1 = 0;
    double State2 = 0;
    double State3 = 0;
    double passbandRipple = 1;
};
