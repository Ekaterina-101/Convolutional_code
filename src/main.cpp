
#include "config.hpp"

// void printResult(int N, std::vector<int>& input_signal, std::vector<int>& output_signal, double param) {
//     int count_err_bit = 0;
//     for (int i = 0; i < input_signal.size(); i++) {
//         if (input_signal[i] != output_signal[i]) {
//             count_err_bit++;
//         }
//     }
//     std::cout << std::fixed << std::setprecision(1) << 
//     "SNR = " << param << std::fixed << std::setprecision(N - 1) << 
//     " pb = " << double(count_err_bit)/input_signal.size() << std::endl;
// }

// std::vector<int> generate_random_vector(size_t size) {
//     std::vector<int> random_vector;
//     srand(time(0));
//     int i = 0;
//     while (i++ < size) {
//         int random_bit = rand() % 2;
//         random_vector.push_back(random_bit);
//     }
//     return random_vector;
// }

int main() {
    Comm_system_config c("config_json/gen.json");
    c.calculate_pe_AWGN();
    c.calculate_pb_AWGN();
}

// int main() {
//     int N = 5;
//     int w_lenght = std::pow(10, N);
//     std::vector<int> w = generate_random_vector(w_lenght);

//     trellis75 T;
//     std::vector<int> encoded = conv75_encode(w);

//     std::cout << "\n#----- QAM-4 hard -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 6.0; SNR += 1.0) {
//         std::vector<std::complex<double>> modulated = QPSKmod(encoded);
//         std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 4);
//         std::vector<int> demodulated = QPSKdemod(noised);
//         std::vector<int> decoded = conv75_viterbi_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//         // std::vector<int> decoded2 = hard_viterbi_decode(demodulated, T);
//         // printResult(N, w, decoded2, SNR);
//     }
        
//     std::cout << "\n#----- QAM-16 hard -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 10.0; SNR += 1.0) {
//         std::vector<std::complex<double>> modulated = QAM16mod(encoded);
//         std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 16);
//         std::vector<int> demodulated = QAM16demod(noised);
//         std::vector<int> decoded = conv75_viterbi_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//         // std::vector<int> decoded2 = hard_viterbi_decode(demodulated, T);
//         // printResult(N, w, decoded2, SNR);
//     }

//     std::cout << "\n#----- QAM-64 hard -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 16.0; SNR += 1.0) {
//         std::vector<std::complex<double>> modulated = QAM64mod(encoded);
//         std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 64);
//         std::vector<int> demodulated = QAM64demod(noised);
//         std::vector<int> decoded = conv75_viterbi_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//     }

//     std::cout << "\n#----- QAM-256 hard -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 24.0; SNR += 1.0) {
//         std::vector<std::complex<double>> modulated = QAM256mod(encoded);
//         std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 256);
//         std::vector<int> demodulated = QAM256demod(noised);
//         std::vector<int> decoded = conv75_viterbi_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//     }

//     // ----------------------------------------------------------------------------------------------------------------------------------------------
//     std::cout << "\n#----- QAM-4 soft -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 6.0; SNR += 1.0) {
//         std::vector<std::complex<double>> modulated = QPSKmod(encoded);
//         std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 4);
//         std::vector<double> demodulated = QPSKdemod_LLR(noised, SNR);
//         std::vector<int> decoded = conv75_soft_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//         // std::vector<int> decoded2 = soft_viterbi_decode(demodulated, T);
//         // printResult(N, w, decoded2, SNR);
//     }
        
//     std::cout << "\n#----- QAM-16 soft -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 10.0; SNR += 1.0) {
//         std::vector<std::complex<double>> modulated = QAM16mod(encoded);
//         std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 16);
//         std::vector<double> demodulated = QAM16demod_LLR(noised, SNR);
//         std::vector<int> decoded = conv75_soft_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//         // std::vector<int> decoded2 = soft_viterbi_decode(demodulated, T);
//         // printResult(N, w, decoded2, SNR);
//     }

//     std::cout << "\n#----- QAM-64 soft -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 14.0; SNR += 1.0) {
//         std::vector<std::complex<double>> modulated = QAM64mod(encoded);
//         std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 64);
//         std::vector<double> demodulated = QAM64demod_LLR(noised, SNR);
//         std::vector<int> decoded = conv75_soft_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//     }

//     std::cout << "\n#----- QAM-256 soft -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 20.0; SNR += 1.0) {
//         std::vector<std::complex<double>> modulated = QAM256mod(encoded);
//         std::vector<std::complex<double>> noised = AWGN_Q(SNR, modulated, 1, 256);
//         std::vector<double> demodulated = QAM256demod_LLR(noised, SNR);
//         std::vector<int> decoded = conv75_soft_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//     }
// }


// int main() {
//     int N = 7;
//     int w_lenght = std::pow(10, N);
//     std::vector<int> w = generate_random_vector(w_lenght);
//     std::vector<int> encoded = conv75_encode(w);

//     std::cout << "\n#----- AWGN (bpsk hard) -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 10.0; SNR += 1.0) {
//         std::vector<double> modulated = BPSKmod(encoded);
//         std::vector<double> noised = AWGN_B(SNR, modulated, 0.5);
//         std::vector<int> demodulated = BPSKdemod(noised);
//         std::vector<int> decoded = conv75_viterbi_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//     }

//     std::cout << "\n#----- AWGN (bpsk soft) -----" << std::endl;
//     for (double SNR = 0.0; SNR <= 10.0; SNR += 1.0) {
//         std::vector<double> modulated = BPSKmod(encoded);
//         std::vector<double> noised = AWGN_B(SNR, modulated, 0.5);
//         std::vector<double> demodulated = BPSKdemod_LLR(noised, SNR);
//         std::vector<int> decoded = conv75_soft_decode(demodulated);
//         printResult(N, w, decoded, SNR);
//     }

//     for (double snr = 0.0; snr <= 12.0 ;snr += 0.5) {
//         double p = Q_func(std::sqrt(2.0 * std::pow(10.0, snr/10))); //  2.0 ?

//         int count_err_word_bsc = 0;
//         int count_err_word_awgn = 0;

//         int word_count = 100000;
//         int word_size = 500;

//         for (int words = 0; words < word_count; words++) {
//             std::vector<int> w = generate_random_vector(word_size);

//             std::vector<int> encoded = conv75_encode(w);
//             std::vector<double> modulated = BPSKmod(encoded);
//             // std::vector<double> BSC_noised = BSC(p, modulated);
//             std::vector<double> AWGN_noised = AWGN_B(snr, modulated, 1);

//             // std::vector<double> BSC_demodulated = BPSKdemod_LLR(BSC_noised, snr);
//             // std::vector<int> BSC_decoded = conv75_soft_decode(BSC_demodulated);
//             // for (int i = 0; i < word_size; i++) {
//             //     if (w[i] != BSC_decoded[i]) {
//             //         count_err_word_bsc++;
//             //         break;
//             //     }
//             // }

//             std::vector<double> AWGN_demodulated = BPSKdemod_LLR(AWGN_noised, snr);
//             std::vector<int> AWGN_decoded = conv75_soft_decode(AWGN_demodulated);
//             for (int i = 0; i < word_size; i++) {
//                 if (w[i] != AWGN_decoded[i]) {
//                     count_err_word_awgn++;
//                     break;
//                 }
//             }
//         }

//         std::cout << std::fixed << std::setprecision(1) << "SNR = " << snr << std::fixed << std::setprecision(4) << " FER = " << (double)count_err_word_bsc / word_count << std::endl;
//         // std::cout << "SNR " << std::fixed << std::setprecision(1) << snr << std::endl;
//         // std::cout << "--BSC--" << std::endl;
//         // std::cout << "FER " << std::fixed << std::setprecision(4) << (double)count_err_word_bsc / word_count << std::endl;
//         // std::cout << "--AWGN--" << std::endl;
//         // std::cout << "FER " << std::fixed << std::setprecision(6) << (double)count_err_word_awgn / word_count << std::endl;
//         // std::cout << std::endl;
//     }
// }
