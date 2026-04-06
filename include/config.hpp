#ifndef COMM_SYSTEM_CONFIG_HPP
#define COMM_SYSTEM_CONFIG_HPP

#include "JSON.hpp"
#include "convolution-coding.hpp"
#include "crc.hpp"
#include "modem.hpp"
#include "noise.hpp"
#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using json = nlohmann::json;

class Comm_system_config {
  private:
    mutable std::mt19937 rng;

    std::string input_json_file;
    std::string output_file;
    std::string output_file_BER_FER;

    int CRC_polynomial;
    int CRC_block;
    int modulation;
    int k;
    int information_word_mode;
    int mode;

    std::vector<uint32_t> trellis_generators;
    Trellis trellis;

    std::string decode_mode;
    int WAVA_max_iter;
    int LVA_L;

    std::string channel;
    int channel_seed;

    std::vector<double> EbN0_dB;
    std::vector<int> attempts;
    int simulation_mode;

    int write_output_BER_FER_file;
    int draw_BER_FER;

    std::vector<int> information_word;
    std::vector<int> information_word_for_crc;
    std::vector<int> code_word;
    std::vector<std::complex<double>> modulation_signal;
    std::vector<std::complex<double>> noised_modulation_signal;
    std::vector<double> soft_demod_LLR;
    std::vector<int> demod_word;
    std::vector<int> decode_word;
    std::vector<std::vector<int>> decode_word_LVA;

    void validate() const;
    void generate_information_word();
    std::vector<int> CRC_code();

  public:
    Comm_system_config();
    explicit Comm_system_config(const std::string &name_input_file);

    void load_from_json(const std::string &filename);

    std::string get_output_file() const;
    int get_CRC_polynomial() const;
    int get_modulation() const;
    int get_k() const;
    int get_mode() const;
    const std::vector<uint32_t> &get_trellis_generators() const;
    const Trellis &get_trellis() const;
    std::string get_decode_mode() const;
    int get_WAVA_max_iter() const;
    int get_LVA_L() const;
    std::string get_channel() const;
    int get_channel_seed() const;
    const std::vector<double> &get_EbN0_dB() const;
    const std::vector<int> &get_attempts() const;
    int get_simulation_mode() const;
    int get_write_BER_FER_file() const;
    int get_draw_BER_FER() const;
    double get_EbN0_linear(double EbN0_db) const;
    int get_attempts_for_point(std::size_t idx) const;

    const std::vector<int> &get_information_word() const;
    const std::vector<int> &get_code_word() const;
    const std::vector<std::complex<double>> &get_modulation_signal() const;
    const std::vector<std::complex<double>> &get_noised_signal() const;
    const std::vector<double> &get_soft_LLR() const;
    const std::vector<int> &get_demod_word() const;
    const std::vector<int> &get_decode_word() const;

    void set_output_file(const std::string &path);
    void set_CRC_polynomial(int poly);
    void set_modulation(int order);
    void set_k(int length);
    void set_mode(int m);
    void set_decode_mode(const std::string &dmode);
    void set_WAVA_max_iter(int iter);
    void set_LVA_L(int L);
    void set_channel(const std::string &ch);
    void set_channel_seed(int seed);
    void set_EbN0_dB(const std::vector<double> &values);
    void set_attempts(const std::vector<int> &vals);
    void set_simulation_mode(int sm);
    void set_information_word_mode(int m);

    std::vector<int> &coding();
    std::vector<std::complex<double>> &modulating();
    std::vector<std::complex<double>> &noise(double current_EbN0_dB);
    std::vector<double> &soft_demod(double current_EbN0_dB);
    std::vector<int> &hard_demod();
    std::vector<int> &decode();

    void draw_BER_FER_curves();
    void print_config() const;

    void run();
};

std::vector<int> generate_random_vector(std::size_t size);
void print_result(int N, std::vector<int> &input_signal,
                  std::vector<int> &output_signal, double param);
void print_vector(const std::vector<int> &vec, std::ostream &file);
void print_vector(const std::vector<double> &vec, std::ostream &file);
void print_complex_vector_compact(const std::vector<std::complex<double>> &vec,
                                  std::ostream &file);

#endif // COMM_SYSTEM_CONFIG_HPP
