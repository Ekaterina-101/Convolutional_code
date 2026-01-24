#include "crc.hpp"
#include "modem.hpp"
#include "noise.hpp"

#include <iostream>
#include <convolution-coding.hpp>

void printResult(std::vector<int>& input_signal, std::vector<int>& output_signal, double param) {
    int count_err_bit = 0;
    for (int i = 0; i < input_signal.size(); i++) {
        if (input_signal[i] != output_signal[i]) {
            count_err_bit++;
        }
    }
    std::cout << std::fixed << std::setprecision(1) << "SNR = " << param << std::fixed << std::setprecision(6) << " pb = " << double(count_err_bit)/input_signal.size() << std::endl;
}

std::vector<int> generate_random_vector(size_t size) {
    std::vector<int> random_vector;
    srand(time(0));
    int i = 0;
    while (i++ < size) {
        int random_bit = rand() % 2;
        random_vector.push_back(random_bit);
    }
    return random_vector;
}

double Q_func(double x) {
    return 0.5 * std::erfc(x / std::sqrt(2.0));
}

int main() {    // некодированная передача
    std::vector<int> w = generate_random_vector(10000000); // 10^-7
    // CRC8 crc;
    // std::vector<int> encoded_CRC = crc.encodeCRC(w);

    // std::cout << "\n #----- BPSK + BSC ----- " << std::endl;
    // for (double SNR = 4.0; SNR <= 12.0; SNR += 0.25) {
    //     double p = Q_func(std::sqrt(std::pow(10.0, SNR/10)));
    //     std::vector<double> modulated = BPSKmod(w);
    //     std::vector<double> noised = BSC(p, modulated);
    //     std::vector<int> demodulated = BPSKdemod(noised);
    //     printResult(w, demodulated, SNR);
    // }

    // std::cout << "\n #----- QAM-2 ----- \nv = 0" << std::endl;
    // for (double SNR = 0.0; SNR <= 24.0; SNR += 0.5) {
    //     std::vector<double> modulated = BPSKmod(w);
    //     size_t length = modulated.size();
    //     std::vector<std::complex<double>> modulated_signal2(length);
    //     // double norm = 1.0 / std::sqrt(2.0);
    //     for (size_t i = 0; i < length; i++) {
    //         modulated_signal2[i].real(modulated[i]);
    //         modulated_signal2[i].imag(0);
    //     }
    //     // std::vector<double> noised = AWGN_Q(SNR, modulated_signal2, 0.5, 2);
    //     std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated_signal2, 0.5, 2);
    //     std::vector<double> noised2(length);
    //     // double norm = 1.0 / std::sqrt(2.0);
    //     for (size_t i = 0; i < length; i++) {
    //         noised2[i] = noised[i].real();
    //     }
    //     std::vector<int> demodulated = BPSKdemod(noised2);
    //     printResult(w, demodulated, SNR);
    // }

    std::cout << "\n#----- QAM-4 ----- \nv = 0" << std::endl;
    for (double SNR = 0.0; SNR <= 12.0; SNR += 1.0) {
        std::vector<std::complex<double>> modulated = QPSKmod(w);
        std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 4);
        std::vector<int> demodulated = QPSKdemod(noised);
        printResult(w, demodulated, SNR);
    }

    std::cout << "\n#----- QAM-16 ----- \nv = 1" << std::endl;
    for (double SNR = 0.0; SNR <= 18.0; SNR += 1.0) {
        std::vector<std::complex<double>> modulated = QAM16mod(w);
        std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 16);
        std::vector<int> demodulated = QAM16demod(noised);
        printResult(w, demodulated, SNR);
    }

    std::cout << "\n#----- QAM-64 ----- \nv = 2" << std::endl;
    for (double SNR = 0.0; SNR <= 25.0; SNR += 1.0) {
        std::vector<std::complex<double>> modulated = QAM64mod(w);
        std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 64);
        std::vector<int> demodulated = QAM64demod(noised);
        printResult(w, demodulated, SNR);
    }

    std::cout << "\n#----- QAM-256 ----- \nv = 3" << std::endl;
    for (double SNR = 0.0; SNR <= 30.0; SNR += 1.0) {
        std::vector<std::complex<double>> modulated = QAM256mod(w);
        std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 256);
        std::vector<int> demodulated = QAM256demod(noised);
        printResult(w, demodulated, SNR);
    }
}

// int main() {     // значения BER convcode(75)
//     for (double snr = 2.0; snr <= 7.0 ;snr += 0.5) {
//         double p = Q_func( std::sqrt(std::pow(10.0, snr/10)) );

//         int count_err_bit_bsc = 0;
//         int count_err_bit_awgn = 0;

//         int word_count = 1;
//         int word_size = 10000000;

//         for (int words = 0; words < word_count; words++) {
//             std::vector<int> w = generate_random_vector(word_size);

//             std::vector<int> encoded = conv75_encode(w);
//             std::vector<double> modulated = BPSKmod(encoded);
//             std::vector<double> BSC_noised = BSC(p, modulated);
//             std::vector<double> AWGN_noised = AWGN_B(snr, modulated);

//             std::vector<int> BSC_demodulated = BPSKdemod(BSC_noised);
//             std::vector<int> BSC_decoded = conv75_viterbi_decode(BSC_demodulated);
//             for (int i = 0; i < word_size; i++) {
//                 if (w[i] != BSC_decoded[i]) {
//                     count_err_bit_bsc++;
//                 }
//             }

//             std::vector<int> AWGN_demodulated = BPSKdemod(AWGN_noised);
//             std::vector<int> AWGN_decoded = conv75_viterbi_decode(AWGN_demodulated);
//             for (int i = 0; i < word_size; i++) {
//                 if (w[i] != AWGN_decoded[i]) {
//                     count_err_bit_awgn++;
//                 }
//             }
//         }

//         std::cout << "SNR = " << std::fixed << std::setprecision(1) << snr << std::endl;
//         std::cout << "--BSC--" << std::endl;
//         std::cout << "BER " << std::fixed << std::setprecision(9) << (double)count_err_bit_bsc / (word_count * word_size) << std::endl;
//         std::cout << "--AWGN--" << std::endl;
//         std::cout << "BER " << std::fixed << std::setprecision(9) << (double)count_err_bit_awgn / (word_count * word_size) << std::endl;
//         std::cout << std::endl;
//     }
// }

// int main() {     // значения FER convcode(75)
//     for (double snr = 2.0; snr <= 7.0 ;snr += 0.5) {
//         double p = Q_func( std::sqrt(std::pow(10.0, snr/10)) );

//         int count_err_word_bsc = 0;
//         int count_err_word_awgn = 0;

//         int word_count = 10000;
//         int word_size = 500;

//         for (int words = 0; words < word_count; words++) {
//             std::vector<int> w = generate_random_vector(word_size);

//             std::vector<int> encoded = conv75_encode(w);
//             std::vector<double> modulated = BPSKmod(encoded);
//             std::vector<double> BSC_noised = BSC(p, modulated);
//             std::vector<double> AWGN_noised = AWGN_B(snr, modulated);

//             std::vector<int> BSC_demodulated = BPSKdemod(BSC_noised);
//             std::vector<int> BSC_decoded = conv75_viterbi_decode(BSC_demodulated);
//             for (int i = 0; i < word_size; i++) {
//                 if (w[i] != BSC_decoded[i]) {
//                     count_err_word_bsc++;
//                     break;
//                 }
//             }

//             std::vector<int> AWGN_demodulated = BPSKdemod(AWGN_noised);
//             std::vector<int> AWGN_decoded = conv75_viterbi_decode(AWGN_demodulated);
//             for (int i = 0; i < word_size; i++) {
//                 if (w[i] != AWGN_decoded[i]) {
//                     count_err_word_awgn++;
//                     break;
//                 }
//             }
//         }

//         std::cout << "SNR " << std::fixed << std::setprecision(1) << snr << std::endl;
//         std::cout << "--BSC--" << std::endl;
//         std::cout << "FER " << std::fixed << std::setprecision(6) << (double)count_err_word_bsc / word_count << std::endl;
//         std::cout << "--AWGN--" << std::endl;
//         std::cout << "FER " << std::fixed << std::setprecision(6) << (double)count_err_word_awgn / word_count << std::endl;
//         std::cout << std::endl;
//     }
// }
