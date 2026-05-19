#include "noise.hpp"

std::vector<double> RayleighFading_B(double SNR, std::vector<double> &word, double R, std::vector<double> &mu_word) {
    mu_word.clear();
    double SNR_linear = std::pow(10.0, SNR / 10.0);
    double σ² = 1.0 / (2.0 * SNR_linear * R);
    double delte = std::sqrt(σ²);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> gauss(0.0, 1.0);

    std::vector<double> result = word;

    for (size_t i = 0; i < word.size(); i++) {
        double x1 = gauss(gen);
        double x2 = gauss(gen);
        double mu = std::sqrt(0.5 * (x1 * x1 + x2 * x2));
        mu_word.push_back(mu);

        result[i] = mu * word[i] + delte * gauss(gen);
    }

    return result;
}

std::vector<std::complex<double>> RayleighFading_Q(double SNR, std::vector<std::complex<double>> &word, double R, double M, std::vector<double> &mu_word, int seed = 123456) {
    mu_word.clear();
    double b = 0.5 * std::pow(10.0, -SNR / 10.0) / (R * std::log2(M));

    std::mt19937 gen(seed);
    double delte = std::sqrt(b);
    std::normal_distribution<double> dist(0.0, 1.0);

    std::vector<std::complex<double>> res = word;

    for (std::size_t i = 0; i < word.size(); i++) {
        double x1 = dist(gen);
        double x2 = dist(gen);
        double mu = std::sqrt(0.5 * (x1 * x1 + x2 * x2));
        mu_word.push_back(mu);

        res[i].real(mu * res[i].real() + delte * dist(gen));
        res[i].imag(mu * res[i].imag() + delte * dist(gen));
    }

    return res;
}
