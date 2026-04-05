#include <vector>
#include <random>
#include <cmath>
#include <complex>

std::vector<std::complex<double>> AWGN_Q(double SNR, std::vector<std::complex<double>>& word, double R, double M, int seed);
std::vector<double> AWGN_B(double SNR, std::vector<double>& word, double R);
std::vector<int> BSC(double p, std::vector<int>& word);
std::vector<double> BSC_soft(double p, std::vector<int>& word);
