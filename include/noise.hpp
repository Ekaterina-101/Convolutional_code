#include <cmath>
#include <complex>
#include <random>
#include <vector>

std::vector<std::complex<double>> AWGN_Q(double SNR, std::vector<std::complex<double>> &word, double R, double M, int seed);
std::vector<double> AWGN_B(double SNR, std::vector<double> &word, double R);
std::vector<int> BSC(double p, std::vector<int> &word);
std::vector<double> BSC_soft(double p, std::vector<int> &word);

std::vector<double> RayleighFading_B(double SNR, std::vector<double> &word, double R, std::vector<double> &mu_word);
std::vector<std::complex<double>> RayleighFading_Q(double SNR, std::vector<std::complex<double>> &word, double R, double M, std::vector<double> &mu_word, int seed);
