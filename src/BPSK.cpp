#include "modem.hpp"

std::vector<double> BPSKmod(const std::vector<int>& input_bits) {
    size_t length = input_bits.size();
    std::vector<double> modulated_signal(length);

    for (size_t i = 0; i < length; i++) {
        modulated_signal[i] = 1 - 2 * input_bits[i];
    }

    return modulated_signal;
}

std::vector<int> BPSKdemod(const std::vector<double>& input_signal) {
    size_t length = input_signal.size();
    std::vector<int> output_bits(length);

    for (size_t i = 0; i < length; i++) {
        if (input_signal[i] < 0) {
            output_bits[i] = 1;
        } else {
            output_bits[i] = 0;
        }
    }

    return output_bits;
}

// double calculateLLR(double SNR_dB, double noised_signal) {
//     double SNR = std::pow(10.0, SNR_dB/10.0);
//     double σ² = 1.0 / (2 * SNR);
//     double result = 2 * noised_signal / σ²;
//     return result;
// }

// std::vector<double> BPSKdemod_LLR(const std::vector<double>& input_signal, double SNR) {
//     size_t length = input_signal.size();
//     std::vector<double> output_bits(length);

//     for (size_t i = 0; i < length; i++) {
//         output_bits[i] = calculateLLR(SNR, input_signal[i]);
//     }

//     return output_bits;
// }

std::vector<double> BPSKdemod_LLR(const std::vector<double>& input_signal, double SNR) {
    size_t length = input_signal.size();
    std::vector<double> output_bits(length);

    for (size_t i = 0; i < length; i++) {
        output_bits[i] = input_signal[i] * 4.0 * std::pow(10.0, SNR/10.0);
    }

    return output_bits;
}
