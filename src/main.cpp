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
    std::cout << std::fixed << std::setprecision(2) << "p/SNR = " << param << std::fixed << std::setprecision(7) << "  BER = " << double(count_err_bit)/input_signal.size() << std::endl;
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

// int main() {
//     std::vector<int> w = generate_random_vector(100000);
//     CRC8 crc;
//     std::vector<int> encoded_CRC = crc.encodeCRC(w);

//     std::cout << "\n ----- BPSK + BSC ----- " << std::endl;
//     for (int p = 0; p < 100; p += 2) {
//         std::vector<double> modulated = BPSKmod(encoded_CRC);
//         std::vector<double> noised = BSC(p/100.0, modulated);
//         std::vector<int> demodulated = BPSKdemod(noised);

//         bool decoded_crc = crc.decodeCRC(demodulated);

//         printResult(w, demodulated, p/100.0);
//     }

//     std::cout << "\n ----- BPSK + AWGN ----- " << std::endl;
//     for (double SNR = 4.0; SNR <= 12.0; SNR += 0.1) {
//         std::vector<double> modulated = BPSKmod(encoded_CRC);
//         std::vector<double> noised = AWGN_B(SNR, modulated);
//         std::vector<int> demodulated = BPSKdemod(noised);

//         bool decoded_crc = crc.decodeCRC(demodulated);

//         printResult(w, demodulated, SNR);
//     }

//     std::cout << "\n ----- QPSK + AWGN ----- " << std::endl;
//     for (double SNR = 4.0; SNR <= 12.0; SNR += 0.1) {
//         // std::vector<std::complex<double>> modulated = QPSKmod(encoded_CRC);
//         std::vector<std::complex<double>> modulated = QAM16mod(encoded_CRC);

//         std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated);
//         // std::vector<int> demodulated = QPSKdemod(noised);
//         std::vector<int> demodulated = QAM16demod(noised);

//         bool CRCdecoded = crc.decodeCRC(demodulated);

//         printResult(w, demodulated, SNR);
//     }
// }

double Q_func(double x) {
    return 0.5 * std::erfc(x / std::sqrt(2.0));
}

int main() {
//     for (double snr = 2.0; snr <= 7.0 ;snr += 0.5) {
//         double p = Q_func( std::sqrt(std::pow(10.0, snr/10)) );

//         // int count_err_bit_bsc = 0;
//         int count_err_word_bsc = 0;
//         // int count_err_bit_awgn = 0;
//         int count_err_word_awgn = 0;

//         int word_count = 10000;
//         int word_size = 500;

//         for (int words = 0; words < word_count; words++) {
//             // bool wrong_word_bsc = false;
//             // bool wrong_word_awgn = false;

//             std::vector<int> w = generate_random_vector(word_size);

//             std::vector<int> encoded = conv75_encode(w);
//             std::vector<double> modulated = BPSKmod(encoded);
//             std::vector<double> BSC_noised = BSC(p, modulated);
//             std::vector<double> AWGN_noised = AWGN_B(snr, modulated);

//             std::vector<int> BSC_demodulated = BPSKdemod(BSC_noised);
//             std::vector<int> BSC_decoded = conv75_viterbi_decode(BSC_demodulated);

//             for (int i = 0; i < word_size; i++) {
//                 if (w[i] != BSC_decoded[i]) {
//                     // count_err_bit_bsc++;
//                     // wrong_word_bsc = true;
//                     count_err_word_bsc++;
//                     break;
//                 }
//             }

//             // if (wrong_word_bsc) {
//             //     count_err_word_bsc++;
//             // }

//             std::vector<int> AWGN_demodulated = BPSKdemod(AWGN_noised);
//             std::vector<int> AWGN_decoded = conv75_viterbi_decode(AWGN_demodulated);

//             for (int i = 0; i < word_size; i++) {
//                 if (w[i] != AWGN_decoded[i]) {
//                     // count_err_bit_awgn++;
//                     // wrong_word_awgn = true;
//                     count_err_word_awgn++;
//                     break;
//                 }
//             }

//             // if (wrong_word_awgn) {
//             //     count_err_word_awgn++;
//             // }
//         }
//         std::cout << "SNR " << std::fixed << std::setprecision(1) << snr << std::endl;

//         std::cout << "--BSC--" << std::endl;
//         // std::cout << "BER " << std::fixed << std::setprecision(9) << (double)count_err_bit_bsc / (word_count * word_size) << std::endl;
//         std::cout << "FER " << std::fixed << std::setprecision(6) << (double)count_err_word_bsc / word_count << std::endl;

//         std::cout << "--AWGN--" << std::endl;
//         // std::cout << "BER " << std::fixed << std::setprecision(9) << (double)count_err_bit_awgn / (word_count * word_size) << std::endl;
//         std::cout << "FER " << std::fixed << std::setprecision(6) << (double)count_err_word_awgn / word_count << std::endl;
//         std::cout << std::endl;
//     }
}
