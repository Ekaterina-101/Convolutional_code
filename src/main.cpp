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
    std::cout << std::fixed << std::setprecision(1) << "SNR = " << param << std::fixed << std::setprecision(7) << " pb = " << double(count_err_bit)/input_signal.size() << std::endl;
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

// int main() {    // некодированная передача
//     std::vector<int> w = generate_random_vector(10000000); // 10^-7
//     // CRC8 crc;
//     // std::vector<int> encoded_CRC = crc.encodeCRC(w);

//     // std::cout << "\n #----- BPSK + BSC -----\n v = 0 " << std::endl; //
//     // for (double SNR = 4.0; SNR <= 12.0; SNR += 0.5) {
//     //     double p = Q_func(std::sqrt(2.0 * std::pow(10.0, SNR/10))); // !
//     //     std::vector<double> modulated = BPSKmod(w);
//     //     std::vector<double> noised = BSC(p, modulated);
//     //     std::vector<int> demodulated = BPSKdemod(noised);
//     //     printResult(w, demodulated, SNR);
//     // }

//     // std::cout << "\n #----- BPSK + AWGN_B -----\n v = 0 " << std::endl; 
//     // for (double SNR = 0.0; SNR <= 14.0; SNR += 1.0) {
//     //     std::vector<double> modulated = BPSKmod(w);
//     //     std::vector<double> noised = AWGN_B(SNR, modulated, 4);
//     //     std::vector<int> demodulated = BPSKdemod(noised);
//     //     printResult(w, demodulated, SNR);
//     // }

    // std::cout << "\n#----- QAM-4 ----- \nv = 1" << std::endl;
    // for (double SNR = 2.0; SNR <= 12.0; SNR += 1.0) {
    //     std::vector<std::complex<double>> modulated = QPSKmod(w);
    //     std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 4);
    //     std::vector<int> demodulated = QPSKdemod(noised);
    //     printResult(w, demodulated, SNR);
    // }

    // std::cout << "\n#----- QAM-16 ----- \nv = 2" << std::endl;
    // for (double SNR = 5.0; SNR <= 21.0; SNR += 1.0) {
    //     std::vector<std::complex<double>> modulated = QAM16mod(w);
    //     std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 16);
    //     std::vector<int> demodulated = QAM16demod(noised);
    //     printResult(w, demodulated, SNR);
    // }

    // std::cout << "\n#----- QAM-64 ----- \nv = 3" << std::endl;
    // for (double SNR = 5.0; SNR <= 27.0; SNR += 1.0) {
    //     std::vector<std::complex<double>> modulated = QAM64mod(w);
    //     std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 64);
    //     std::vector<int> demodulated = QAM64demod(noised);
    //     printResult(w, demodulated, SNR);
    // }

    // std::cout << "\n#----- QAM-256 ----- \nv = 3" << std::endl;
    // for (double SNR = 5.0; SNR <= 33.0; SNR += 1.0) {
    //     std::vector<std::complex<double>> modulated = QAM256mod(w);
    //     std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 256);
    //     std::vector<int> demodulated = QAM256demod(noised);
    //     printResult(w, demodulated, SNR);
    // }
// }

int main() {     // значения BER convcode(75)
    // for (double snr = 0.0; snr <= 6.0 ;snr += 0.5) {
    //     double p = Q_func(std::sqrt(2.0 * std::pow(10.0, snr/10)));

    //     int count_err_bit_bsc = 0;
    //     int count_err_bit_awgn = 0;

    //     int word_count = 1;
    //     int word_size = 100000;

    //     for (int words = 0; words < word_count; words++) {
    //         std::vector<int> w = generate_random_vector(word_size);

    //         std::vector<int> encoded = conv75_encode(w);
    //         std::vector<double> modulated = BPSKmod(encoded);
    //         std::vector<double> BSC_noised = BSC(p, modulated);
    //         std::vector<double> AWGN_noised = AWGN_B(snr, modulated, 1);

    //         std::vector<int> BSC_demodulated = BPSKdemod(BSC_noised);
    //         std::vector<int> BSC_decoded = conv75_viterbi_decode(BSC_demodulated);
    //         for (int i = 0; i < word_size; i++) {
    //             if (w[i] != BSC_decoded[i]) {
    //                 count_err_bit_bsc++;
    //             }
    //         }

    //         std::vector<int> AWGN_demodulated = BPSKdemod(AWGN_noised);
    //         std::vector<int> AWGN_decoded = conv75_viterbi_decode(AWGN_demodulated);
    //         for (int i = 0; i < word_size; i++) {
    //             if (w[i] != AWGN_decoded[i]) {
    //                 count_err_bit_awgn++;
    //             }
    //         }
    //     }

    //     std::cout << "SNR = " << std::fixed << std::setprecision(1) << snr << std::endl;
    //     std::cout << "--BSC--" << std::endl;
    //     std::cout << "BER " << std::fixed << std::setprecision(9) << (double)count_err_bit_bsc / (word_count * word_size) << std::endl;
    //     std::cout << "--AWGN--" << std::endl;
    //     std::cout << "BER " << std::fixed << std::setprecision(9) << (double)count_err_bit_awgn / (word_count * word_size) << std::endl;
    //     std::cout << std::endl;
    // }
    std::vector<int> w = generate_random_vector(100000000); // 10^-7
    std::vector<int> encoded = conv75_encode(w);

    // std::cout << "\n#----- BSC hard ----- (синий)\nv = 0 " << std::endl;
    // for (double SNR = 2.0; SNR <= 7.0; SNR += 0.5) {
    //     double p = Q_func(std::sqrt(2.0 * std::pow(10.0, SNR/10))); // !
    //     std::vector<double> modulated = BPSKmod(encoded);
    //     std::vector<double> noised = BSC(p, modulated);
    //     std::vector<int> demodulated = BPSKdemod(noised);
    //     std::vector<int> decoded = conv75_viterbi_decode(demodulated);
    //     printResult(w, decoded, SNR);
    // }

    std::cout << "\n#----- BSC soft ----- (оранжевый)\nv = 1 " << std::endl;
    for (double SNR = 2.0; SNR <= 7.0; SNR += 0.5) {
        double p = Q_func(std::sqrt(2.0 * std::pow(10.0, SNR/10))); // !
        std::vector<double> modulated = BPSKmod(encoded);
        std::vector<double> noised = BSC(p, modulated);
        std::vector<double> demodulated = BPSKdemod_LLR(noised, SNR);
        std::vector<int> decoded = conv75_soft_decode(demodulated);
        printResult(w, decoded, SNR);
    }

    // std::cout << "\n#----- AWGN hard ----- (зеленка)\nv = 2 " << std::endl;
    // for (double SNR = 2.0; SNR <= 7.0; SNR += 0.5) {
    //     std::vector<double> modulated = BPSKmod(encoded);
    //     std::vector<double> noised = AWGN_B(SNR, modulated, 1);
    //     std::vector<int> demodulated = BPSKdemod(noised);
    //     std::vector<int> decoded = conv75_viterbi_decode(demodulated);
    //     printResult(w, decoded, SNR);
    // }

    std::cout << "\n#----- AWGN soft ----- (красный)\nv = 3 " << std::endl;
    for (double SNR = 2.0; SNR <= 7.0; SNR += 0.5) {
        std::vector<double> modulated = BPSKmod(encoded);
        std::vector<double> noised = AWGN_B(SNR, modulated, 1);
        std::vector<double> demodulated = BPSKdemod_LLR(noised, SNR);
        std::vector<int> decoded = conv75_soft_decode(demodulated);
        printResult(w, decoded, SNR);
    }

    for (double snr = 2.0; snr <= 7.0 ;snr += 0.5) {
        double p = Q_func(std::sqrt(2.0 * std::pow(10.0, snr/10))); ///////////////////////////////// 2.0 ?

        int count_err_word_bsc = 0;
        int count_err_word_awgn = 0;

        int word_count = 100000;
        int word_size = 500;

        for (int words = 0; words < word_count; words++) {
            std::vector<int> w = generate_random_vector(word_size);

            std::vector<int> encoded = conv75_encode(w);
            std::vector<double> modulated = BPSKmod(encoded);
            std::vector<double> BSC_noised = BSC(p, modulated);
            std::vector<double> AWGN_noised = AWGN_B(snr, modulated, 1);

            std::vector<double> BSC_demodulated = BPSKdemod_LLR(BSC_noised, snr);
            std::vector<int> BSC_decoded = conv75_soft_decode(BSC_demodulated);
            for (int i = 0; i < word_size; i++) {
                if (w[i] != BSC_decoded[i]) {
                    count_err_word_bsc++;
                    break;
                }
            }

            std::vector<double> AWGN_demodulated = BPSKdemod_LLR(AWGN_noised, snr);
            std::vector<int> AWGN_decoded = conv75_soft_decode(AWGN_demodulated);
            for (int i = 0; i < word_size; i++) {
                if (w[i] != AWGN_decoded[i]) {
                    count_err_word_awgn++;
                    break;
                }
            }
        }

        std::cout << "SNR " << std::fixed << std::setprecision(1) << snr << std::endl;
        std::cout << "--BSC--" << std::endl;
        std::cout << "FER " << std::fixed << std::setprecision(6) << (double)count_err_word_bsc / word_count << std::endl;
        std::cout << "--AWGN--" << std::endl;
        std::cout << "FER " << std::fixed << std::setprecision(6) << (double)count_err_word_awgn / word_count << std::endl;
        std::cout << std::endl;
    }
}

// int main() {     // значения FER convcode(75)
//     for (double snr = 2.0; snr <= 7.0 ;snr += 0.5) {
//         double p = Q_func( std::sqrt(std::pow(10.0, snr/10)) ); ///////////////////////////////// 2.0 ?

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

// int main() {
//         srand(time(0));
//         int random_size = rand() % 10000;

//         int N = random_size;
//         auto info = generate_random_vector(N);
//         auto encoded = conv75_encode(info);

//         std::vector<std::complex<double>> modulated = QAM256mod(encoded);
//         std::vector<std::complex<double>> noised = AWGN_Q(10, modulated, 1, 256);
//         std::vector<int> demodulated = QAM256demod(noised);

//         auto decoded = conv75_viterbi_decode(demodulated);

//         int counter = 0;
//         for (int i = 0; i < decoded.size(); i++) {
//             if (info[i] != decoded[i]) {
//                 counter++;
//             }
//         }

//         std::cout << counter << "/" << N << std::endl;
// }
