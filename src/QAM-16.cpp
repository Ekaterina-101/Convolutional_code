#include "modem.hpp"

std::vector<std::complex<double>> QAM16mod(const std::vector<int>& input_bits) {
    size_t length = input_bits.size() / 4;
    std::vector<std::complex<double>> modulated_signal(length);

    double norm = 1.0 / std::sqrt(10.0);
    
    for (size_t i = 0; i < length; i++) {
        int b0 = input_bits[4 * i];
        int b1 = input_bits[4 * i + 1];
        int b2 = input_bits[4 * i + 2];
        int b3 = input_bits[4 * i + 3];

        double real_part = (1 - 2 * b0) * (1 + 2 * b1) * norm;
        double imag_part = (1 - 2 * b2) * (1 + 2 * b3) * norm;
        
        modulated_signal[i].real(real_part);
        modulated_signal[i].imag(imag_part);
    }

    return modulated_signal;
}

std::vector<int> QAM16demod(const std::vector<std::complex<double>>& input_signal) {
    size_t length = input_signal.size();
    std::vector<int> output_bits(length * 4);

    double norm = 1.0 / std::sqrt(10.0);
    
    for (size_t i = 0; i < length; i++) {
        std::vector<double> distances(16);
        
        distances[0] = std::abs(input_signal[i] - std::complex<double>(1.0 * norm, 1.0 * norm));   // 0000
        distances[1] = std::abs(input_signal[i] - std::complex<double>(1.0 * norm, 3.0 * norm));   // 0001
        distances[2] = std::abs(input_signal[i] - std::complex<double>(1.0 * norm, -1.0 * norm));  // 0010
        distances[3] = std::abs(input_signal[i] - std::complex<double>(1.0 * norm, -3.0 * norm));  // 0011
        distances[4] = std::abs(input_signal[i] - std::complex<double>(3.0 * norm, 1.0 * norm));   // 0100
        distances[5] = std::abs(input_signal[i] - std::complex<double>(3.0 * norm, 3.0 * norm));   // 0101
        distances[6] = std::abs(input_signal[i] - std::complex<double>(3.0 * norm, -1.0 * norm));  // 0110
        distances[7] = std::abs(input_signal[i] - std::complex<double>(3.0 * norm, -3.0 * norm));  // 0111
        distances[8] = std::abs(input_signal[i] - std::complex<double>(-1.0 * norm, 1.0 * norm));  // 1000
        distances[9] = std::abs(input_signal[i] - std::complex<double>(-1.0 * norm, 3.0 * norm));  // 1001
        distances[10] = std::abs(input_signal[i] - std::complex<double>(-1.0 * norm, -1.0 * norm)); // 1010
        distances[11] = std::abs(input_signal[i] - std::complex<double>(-1.0 * norm, -3.0 * norm)); // 1011
        distances[12] = std::abs(input_signal[i] - std::complex<double>(-3.0 * norm, 1.0 * norm));  // 1100
        distances[13] = std::abs(input_signal[i] - std::complex<double>(-3.0 * norm, 3.0 * norm));  // 1101
        distances[14] = std::abs(input_signal[i] - std::complex<double>(-3.0 * norm, -1.0 * norm)); // 1110
        distances[15] = std::abs(input_signal[i] - std::complex<double>(-3.0 * norm, -3.0 * norm)); // 1111

        int best_idx = std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()));

        output_bits[4 * i] = (best_idx >> 3) & 1;
        output_bits[4 * i + 1] = (best_idx >> 2) & 1;
        output_bits[4 * i + 2] = (best_idx >> 1) & 1;
        output_bits[4 * i + 3] = best_idx & 1;
    }

    return output_bits;
}
