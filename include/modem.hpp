#include <algorithm>
#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>
#include <vector>

std::vector<double> BPSKmod(const std::vector<int> &input_bits);
std::vector<std::complex<double>> QPSKmod(const std::vector<int> &input_bits);
std::vector<std::complex<double>> QAM16mod(const std::vector<int> &input_bits);
std::vector<std::complex<double>> QAM64mod(const std::vector<int> &input_bits);
std::vector<std::complex<double>> QAM256mod(const std::vector<int> &input_bits);

std::vector<int> BPSKdemod(const std::vector<double> &input_signal);
std::vector<int> QPSKdemod(const std::vector<std::complex<double>> &input_signal);
std::vector<int> QAM16demod(const std::vector<std::complex<double>> &input_signal);
std::vector<int> QAM64demod(const std::vector<std::complex<double>> &input_signal);
std::vector<int> QAM256demod(const std::vector<std::complex<double>> &input_signal);

double calculateLLR(double SNR_dB, double min_dist_difference, double R, double M);
std::vector<double> BPSKdemod_LLR(const std::vector<double> &input_signal, double SNR_dB, double R);
std::vector<double> QPSKdemod_LLR(const std::vector<std::complex<double>> &input_signal, double SNR_dB, double R);
std::vector<double> QAM16demod_LLR(const std::vector<std::complex<double>> &input_signal, double SNR_dB, double R);
std::vector<double> QAM64demod_LLR(const std::vector<std::complex<double>> &input_signal, double SNR_dB, double R);
std::vector<double> QAM256demod_LLR(const std::vector<std::complex<double>> &input_signal, double SNR_dB, double R);

std::vector<double> BPSKdemod_LLR_Rayleigh(const std::vector<double> &input_signal, const std::vector<double> &fading_mu, double SNR, double R);
std::vector<double> BPSKdemod_LLR_Rayleigh_NSI(const std::vector<double> &input_signal, double SNR, double R);
std::vector<double> QPSKdemod_LLR_Rayleigh(const std::vector<std::complex<double>> &input_signal, const std::vector<double> &fading_mu, double SNR, double R);
std::vector<double> QAM16demod_LLR_Rayleigh(const std::vector<std::complex<double>> &input_signal, const std::vector<double> &fading_mu, double SNR, double R);
std::vector<double> QAM64demod_LLR_Rayleigh(const std::vector<std::complex<double>> &input_signal, const std::vector<double> &fading_mu, double SNR, double R);
std::vector<double> QAM256demod_LLR_Rayleigh(const std::vector<std::complex<double>> &input_signal, const std::vector<double> &fading_mu, double SNR, double R);
