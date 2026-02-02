#include "modem.hpp"

std::vector<std::complex<double>> QAM256mod(const std::vector<int>& input_bits) {
    size_t length = input_bits.size() / 8;
    std::vector<std::complex<double>> modulated_signal(length);

    double norm = 1.0 / std::sqrt(680.0);
    
    for (size_t i = 0; i < length; i++) {
        int b0 = input_bits[8 * i];
        int b1 = input_bits[8 * i + 1];
        int b2 = input_bits[8 * i + 2];
        int b3 = input_bits[8 * i + 3];
        int b4 = input_bits[8 * i + 4];
        int b5 = input_bits[8 * i + 5];
        int b6 = input_bits[8 * i + 6];
        int b7 = input_bits[8 * i + 7];

        double real_part = (1 - 2 * b0) * (1 + 2 * b1 + 4 * b2 + 8 * b3) * norm;
        double imag_part = (1 - 2 * b4) * (1 + 2 * b5 + 4 * b6 + 8 * b7) * norm;

        modulated_signal[i].real(real_part);
        modulated_signal[i].imag(imag_part);
    }

    return modulated_signal;
}

std::vector<int> QAM256demod(const std::vector<std::complex<double>>& input_signal) {
    size_t length = input_signal.size();
    std::vector<int> output_bits(length * 8);

    double norm = 1.0 / std::sqrt(680.0);
    
    for (size_t i = 0; i < length; i++) {
        double min_distance = std::numeric_limits<double>::max();
        int best_idx = 0;
        
        for (int b0 = 0; b0 <= 1; b0++) {
            for (int b1 = 0; b1 <= 1; b1++) {
                for (int b2 = 0; b2 <= 1; b2++) {
                    for (int b3 = 0; b3 <= 1; b3++) {
                        for (int b4 = 0; b4 <= 1; b4++) {
                            for (int b5 = 0; b5 <= 1; b5++) {
                                for (int b6 = 0; b6 <= 1; b6++) {
                                    for (int b7 = 0; b7 <= 1; b7++) {
                                        double real_part = (1 - 2 * b0) * (1 + 2 * b1 + 4 * b2 + 8 * b3) * norm;
                                        double imag_part = (1 - 2 * b4) * (1 + 2 * b5 + 4 * b6 + 8 * b7) * norm;
                                        
                                        double distance = std::abs(input_signal[i] - std::complex<double>(real_part, imag_part));
                                        
                                        if (distance < min_distance) {
                                            min_distance = distance;
                                            best_idx = (b0 << 7) | (b1 << 6) | (b2 << 5) | (b3 << 4) | (b4 << 3) | (b5 << 2) | (b6 << 1) | b7;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        output_bits[8 * i] = (best_idx >> 7) & 1;
        output_bits[8 * i + 1] = (best_idx >> 6) & 1;
        output_bits[8 * i + 2] = (best_idx >> 5) & 1;
        output_bits[8 * i + 3] = (best_idx >> 4) & 1;
        output_bits[8 * i + 4] = (best_idx >> 3) & 1;
        output_bits[8 * i + 5] = (best_idx >> 2) & 1;
        output_bits[8 * i + 6] = (best_idx >> 1) & 1;
        output_bits[8 * i + 7] = best_idx & 1;
    }

    return output_bits;
}
