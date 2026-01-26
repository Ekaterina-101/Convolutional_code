#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>
#include <iostream>
#include <iomanip>

std::vector<double> BPSKmod(const std::vector<int>& input_bits);
std::vector<int> BPSKdemod(const std::vector<double>& input_signal);

std::vector<std::complex<double>> QPSKmod(const std::vector<int>& input_bits);
std::vector<int> QPSKdemod(const std::vector<std::complex<double>>& input_signal);

std::vector<std::complex<double>> QAM16mod(const std::vector<int>& input_bits);
std::vector<int> QAM16demod(const std::vector<std::complex<double>>& input_signal);

std::vector<std::complex<double>> QAM64mod(const std::vector<int>& input_bits);
std::vector<int> QAM64demod(const std::vector<std::complex<double>>& input_signal);

std::vector<std::complex<double>> QAM256mod(const std::vector<int>& input_bits);
std::vector<int> QAM256demod(const std::vector<std::complex<double>>& input_signal);

std::vector<double> QPSKdemod_LLR(const std::vector<std::complex<double>>& input_signal, double SNR);
std::vector<double> BPSKdemod_LLR(const std::vector<double>& input_signal, double SNR);
