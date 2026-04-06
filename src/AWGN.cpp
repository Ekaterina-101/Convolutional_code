#include "noise.hpp"

std::vector<std::complex<double>> AWGN_Q(double SNR, std::vector<std::complex<double>>& word, double R, double M, int seed = 123456) {
    double b = 0.5 * std::pow(10.0, -SNR/10.0) / (R * std::log2(M));

    std::mt19937 gen(seed);
    double delte = std::sqrt(b);
    std::normal_distribution<double> dist(0.0, 1.0);

    std::vector<std::complex<double>> res = word;

    for (std::size_t i = 0; i < word.size(); i++) {
        res[i].real(res[i].real() + delte * dist(gen));
        res[i].imag(res[i].imag() + delte * dist(gen));
    }

    return res;
}

std::vector<double> AWGN_B(double SNR, std::vector<double>& word, double R) {
    double b = 0.5 * std::pow(10.0, -SNR/10.0) / R;

    std::random_device rd;
    std::mt19937 gen(rd());
    double delte = std::sqrt(b);
    std::normal_distribution<double> dist(0.0, 1.0);

    std::vector<double> res = word;

    for (std::size_t i = 0; i < word.size(); i++) {
        res[i] += delte * dist(gen);
    }

    return res;
}
