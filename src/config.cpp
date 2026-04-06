#include "config.hpp"
#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

std::vector<int> generate_random_vector(std::mt19937 &rng, std::size_t size) {
    std::vector<int> random_vector;
    random_vector.reserve(size);
    std::uniform_int_distribution<int> dist(0, 1);
    for (std::size_t i = 0; i < size; ++i) {
        random_vector.push_back(dist(rng));
    }
    return random_vector;
}

void print_result(int N, std::vector<int> &input_signal,
                  std::vector<int> &output_signal, double param) {
    int count_err_bit = 0;
    for (std::size_t i = 0; i < input_signal.size(); i++) {
        if (input_signal[i] != output_signal[i]) {
            count_err_bit++;
        }
    }
    std::cout << std::fixed << std::setprecision(1) << "Eb/N0 = " << param
              << std::fixed << std::setprecision(N - 1)
              << " pb = " << double(count_err_bit) / input_signal.size()
              << std::endl;
}

void print_vector(const std::vector<int> &vec, std::ostream &file) {
    for (std::size_t i = 0; i < vec.size(); ++i) {
        file << vec[i];
        if (i < vec.size() - 1) {
            file << " ";
        }
    }
    file << std::endl;
    file << std::endl;
}

void print_vector(const std::vector<double> &vec, std::ostream &file) {
    for (size_t i = 0; i < vec.size(); ++i) {
        file << vec[i];
        if (i < vec.size() - 1) {
            file << " ";
        }
    }
    file << std::endl;
    file << std::endl;
}

void print_complex_vector_compact(const std::vector<std::complex<double>> &vec,
                                  std::ostream &file) {
    file << "[";
    for (std::size_t i = 0; i < vec.size(); ++i) {
        file << vec[i].real() << (vec[i].imag() >= 0 ? "+" : "")
             << vec[i].imag() << "i";
        if (i < vec.size() - 1)
            file << ", ";
    }
    file << "]" << std::endl;
    file << std::endl;
}

Comm_system_config::Comm_system_config()
    : input_json_file("config_json/exmp.json"), output_file("data/output.log"),
      CRC_polynomial(8), CRC_block(0), modulation(4), k(10),
      information_word_mode(0), mode(0), decode_mode("soft"), WAVA_max_iter(3),
      LVA_L(5), channel("AWGN"), channel_seed(12345), EbN0_dB({1.0, 1.5, 3.0}),
      attempts({1000, 500, 200}), simulation_mode(0),
      write_output_BER_FER_file(0), draw_BER_FER(0), trellis_generators({7, 5}),
      rng(channel_seed) {
    trellis = build_trellis(trellis_generators);
}

Comm_system_config::Comm_system_config(const std::string &name_input_file)
    : input_json_file(name_input_file), rng(12345) {
    load_from_json(name_input_file);
}

void Comm_system_config::load_from_json(const std::string &filename) {
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cerr << "Error: can't open input file '" << filename << "'"
                  << std::endl;
        return;
    }

    json input_json;
    try {
        input_json = json::parse(input_file);
    } catch (const json::parse_error &e) {
        std::cerr << "Error: JSON parse error at byte " << e.byte << ": "
                  << e.what() << std::endl;
        return;
    }

    if (input_json.contains("output_file") &&
        input_json["output_file"].is_string()) {
        output_file = input_json["output_file"].get<std::string>();
    }
    if (output_file.empty()) {
        std::time_t now = std::time(nullptr);
        std::tm tm{};
        localtime_r(&now, &tm);

        std::ostringstream oss;
        oss << "data/output_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".log";
        output_file = oss.str();
    }
    std::filesystem::path p(output_file);
    if (p.has_parent_path() && !p.parent_path().empty()) {
        std::filesystem::create_directories(p.parent_path());
    }
    std::ofstream file(output_file);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + output_file);
    }

    if (input_json.contains("CRC_polynomial") &&
        input_json["CRC_polynomial"].is_number_integer()) {
        CRC_polynomial = input_json["CRC_polynomial"].get<int>();
    }

    if (input_json.contains("CRC_block") &&
        input_json["CRC_block"].is_number_integer()) {
        CRC_block = input_json["CRC_block"].get<int>();
    }

    if (input_json.contains("modulation") &&
        input_json["modulation"].is_number_integer()) {
        modulation = input_json["modulation"].get<int>();
    }

    if (input_json.contains("k") && input_json["k"].is_number_integer()) {
        k = input_json["k"].get<int>();
    }

    if (input_json.contains("information_word") &&
        input_json["information_word"].is_number_integer()) {
        information_word_mode = input_json["information_word"].get<int>();
    }

    if (input_json.contains("mode") && input_json["mode"].is_number_integer()) {
        mode = input_json["mode"].get<int>();
    }

    if (input_json.contains("trellis_generators") &&
        input_json["trellis_generators"].is_array()) {
        trellis_generators =
            json_to_vector_int32(input_json["trellis_generators"]);
        trellis = build_trellis(trellis_generators);
    }

    if (input_json.contains("decode_mode") &&
        input_json["decode_mode"].is_string()) {
        decode_mode = input_json["decode_mode"].get<std::string>();
    }

    if (input_json.contains("WAVA_max_iter") &&
        input_json["WAVA_max_iter"].is_number_integer()) {
        WAVA_max_iter = input_json["WAVA_max_iter"].get<int>();
    }

    if (input_json.contains("LVA_L") &&
        input_json["LVA_L"].is_number_integer()) {
        LVA_L = input_json["LVA_L"].get<int>();
    }

    if (input_json.contains("channel") && input_json["channel"].is_string()) {
        channel = input_json["channel"].get<std::string>();
    }

    if (input_json.contains("channel_seed") &&
        input_json["channel_seed"].is_number_integer()) {
        channel_seed = input_json["channel_seed"].get<int>();
        rng.seed(channel_seed);
    }

    if (input_json.contains("EbN0_dB")) {
        if (input_json["EbN0_dB"].is_number()) {
            EbN0_dB = {input_json["EbN0_dB"].get<double>()};
        } else if (input_json["EbN0_dB"].is_array()) {
            EbN0_dB.clear();
            for (auto &val : input_json["EbN0_dB"]) {
                if (val.is_number()) {
                    EbN0_dB.push_back(val.get<double>());
                }
            }
        }
    }

    if (input_json.contains("attempts")) {
        if (input_json["attempts"].is_number_integer()) {
            attempts = {input_json["attempts"].get<int>()};
        } else if (input_json["attempts"].is_array()) {
            attempts.clear();
            for (const auto &val : input_json["attempts"]) {
                if (val.is_number_integer()) {
                    attempts.push_back(val.get<int>());
                }
            }
        }
    }

    if (input_json.contains("simulation_mode") &&
        input_json["simulation_mode"].is_number_integer()) {
        simulation_mode = input_json["simulation_mode"].get<int>();
    }

    if (input_json.contains("write_output_BER/FER_file") &&
        input_json["write_output_BER/FER_file"].is_number_integer()) {
        write_output_BER_FER_file =
            input_json["write_output_BER/FER_file"].get<int>();
    }
    if (input_json.contains("draw_BER/FER") &&
        input_json["draw_BER/FER"].is_number_integer()) {
        draw_BER_FER = input_json["draw_BER/FER"].get<int>();
    }

    validate();
    generate_information_word();
}

std::string Comm_system_config::get_output_file() const { return output_file; }
int Comm_system_config::get_CRC_polynomial() const { return CRC_polynomial; }
int Comm_system_config::get_modulation() const { return modulation; }
int Comm_system_config::get_k() const { return k; }
int Comm_system_config::get_mode() const { return mode; }
const std::vector<uint32_t> &
Comm_system_config::get_trellis_generators() const {
    return trellis_generators;
}
const Trellis &Comm_system_config::get_trellis() const { return trellis; }
std::string Comm_system_config::get_decode_mode() const { return decode_mode; }
int Comm_system_config::get_WAVA_max_iter() const { return WAVA_max_iter; }
int Comm_system_config::get_LVA_L() const { return LVA_L; }
std::string Comm_system_config::get_channel() const { return channel; }
int Comm_system_config::get_channel_seed() const { return channel_seed; }
const std::vector<double> &Comm_system_config::get_EbN0_dB() const {
    return EbN0_dB;
}
const std::vector<int> &Comm_system_config::get_attempts() const {
    return attempts;
}
int Comm_system_config::get_simulation_mode() const { return simulation_mode; }
int Comm_system_config::get_write_BER_FER_file() const {
    return write_output_BER_FER_file;
}
int Comm_system_config::get_draw_BER_FER() const { return draw_BER_FER; }

const std::vector<int> &Comm_system_config::get_information_word() const {
    return information_word;
}
const std::vector<int> &Comm_system_config::get_code_word() const {
    return code_word;
}
const std::vector<std::complex<double>> &
Comm_system_config::get_modulation_signal() const {
    return modulation_signal;
}
const std::vector<std::complex<double>> &
Comm_system_config::get_noised_signal() const {
    return noised_modulation_signal;
}
const std::vector<double> &Comm_system_config::get_soft_LLR() const {
    return soft_demod_LLR;
}
const std::vector<int> &Comm_system_config::get_demod_word() const {
    return demod_word;
}
const std::vector<int> &Comm_system_config::get_decode_word() const {
    return decode_word;
}

void Comm_system_config::set_output_file(const std::string &path) {
    output_file = path;
}
void Comm_system_config::set_CRC_polynomial(int poly) {
    CRC_polynomial = poly;
    validate();
}
void Comm_system_config::set_modulation(int order) {
    modulation = order;
    validate();
}
void Comm_system_config::set_k(int length) {
    k = length;
    validate();
}
void Comm_system_config::set_mode(int m) {
    mode = m;
    validate();
}
void Comm_system_config::set_decode_mode(const std::string &dmode) {
    decode_mode = dmode;
    validate();
}
void Comm_system_config::set_WAVA_max_iter(int iter) { WAVA_max_iter = iter; }
void Comm_system_config::set_LVA_L(int L) { LVA_L = L; }
void Comm_system_config::set_channel(const std::string &ch) {
    channel = ch;
    validate();
}
void Comm_system_config::set_channel_seed(int seed) {
    channel_seed = seed;
    rng.seed(seed);
}
void Comm_system_config::set_EbN0_dB(const std::vector<double> &values) {
    EbN0_dB = values;
    validate();
}
void Comm_system_config::set_attempts(const std::vector<int> &vals) {
    attempts = vals;
    validate();
}
void Comm_system_config::set_simulation_mode(int sm) {
    simulation_mode = sm;
    validate();
}

void Comm_system_config::set_information_word_mode(int m) {
    information_word_mode = m;
    generate_information_word();
}

double Comm_system_config::get_EbN0_linear(double EbN0_db) const {
    return std::pow(10.0, EbN0_db / 10.0);
}

int Comm_system_config::get_attempts_for_point(std::size_t idx) const {
    if (simulation_mode == 0 || idx >= attempts.size()) {
        return attempts.empty() ? 1000 : attempts[0];
    }
    return attempts[idx];
}

void Comm_system_config::generate_information_word() {
    if (information_word_mode == 0) {
        information_word.assign(k, 0);
    } else {
        information_word = generate_random_vector(rng, k);
    }
}

std::vector<int> &Comm_system_config::coding() {
    std::vector<int> word_to_encode = information_word;
    if (mode == 5) {
        word_to_encode = CRC_code();
    }
    // if ((decode_mode.find("WAVA") != std::string::npos)) {
    //     code_word = tail_biting_encode(trellis, information_word);
    // } else {
    //     code_word = conv_encode(trellis, information_word);
    // }
    code_word = conv_encode(trellis, word_to_encode);
    return code_word;
}

std::vector<std::complex<double>> &Comm_system_config::modulating() {
    switch (modulation) {
    case 4:
        modulation_signal = QPSKmod(code_word);
        break;
    case 16:
        modulation_signal = QAM16mod(code_word);
        break;
    case 64:
        modulation_signal = QAM64mod(code_word);
        break;
    case 256:
        modulation_signal = QAM256mod(code_word);
        break;
    default:
        modulation_signal = QPSKmod(code_word);
        break;
    }
    return modulation_signal;
}

std::vector<std::complex<double>> &
Comm_system_config::noise(double current_EbN0_dB) {
    double R = 1.0 / static_cast<double>(trellis_generators.size());

    if (channel == "AWGN") {
        noised_modulation_signal = AWGN_Q(current_EbN0_dB, modulation_signal, R,
                                          modulation, channel_seed);
    }
    // else if (channel == "RYLV") {
    //    noised_modulation_signal =
    //        Rayleigh_channel(current_EbN0_dB, modulation_signal, R,
    //                         modulation, channel_seed);
    //  }
    return noised_modulation_signal;
}

std::vector<double> &Comm_system_config::soft_demod(double current_EbN0_dB) {
    switch (modulation) {
    case 4:
        soft_demod_LLR =
            QPSKdemod_LLR(noised_modulation_signal, current_EbN0_dB);
        break;
    case 16:
        soft_demod_LLR =
            QAM16demod_LLR(noised_modulation_signal, current_EbN0_dB);
        break;
    case 64:
        soft_demod_LLR =
            QAM64demod_LLR(noised_modulation_signal, current_EbN0_dB);
        break;
    case 256:
        soft_demod_LLR =
            QAM256demod_LLR(noised_modulation_signal, current_EbN0_dB);
        break;
    default:
        soft_demod_LLR =
            QPSKdemod_LLR(noised_modulation_signal, current_EbN0_dB);
        break;
    }
    return soft_demod_LLR;
}

std::vector<int> &Comm_system_config::hard_demod() {
    switch (modulation) {
    case 4:
        demod_word = QPSKdemod(noised_modulation_signal);
        break;
    case 16:
        demod_word = QAM16demod(noised_modulation_signal);
        break;
    case 64:
        demod_word = QAM64demod(noised_modulation_signal);
        break;
    case 256:
        demod_word = QAM256demod(noised_modulation_signal);
        break;
    default:
        demod_word = QPSKdemod(noised_modulation_signal);
        break;
    }
    return demod_word;
}

std::vector<int> &Comm_system_config::decode() {
    bool is_soft = (decode_mode.find("soft") != std::string::npos);
    bool is_WAVA = (decode_mode.find("WAVA") != std::string::npos);
    bool is_LVA = (decode_mode.find("LVA") != std::string::npos);
    bool is_parallel = (decode_mode.find("parallel") != std::string::npos);
    bool is_serial = (decode_mode.find("serial") != std::string::npos);

    // if (is_WAVA) {
    //   if (is_soft) {
    //     decode_word = soft_wava_decode(trellis, soft_demod_LLR,
    //     WAVA_max_iter);
    //   } else {
    //     decode_word = hard_wava_decode(trellis, demod_word,
    //     WAVA_max_iter);
    //   }
    // } else if (is_LVA) {
    //   if (is_parallel) {
    //     if (is_soft) {
    //       decode_word_LVA =
    //           soft_parallel_lva(trellis, soft_demod_LLR, LVA_L);
    //     } else {
    //       decode_word_LVA = hard_parallel_lva(trellis, demod_word,
    //       LVA_L);
    //     }
    //   } else {
    //     if (is_soft) {
    //       decode_word_LVA = soft_serial_lva(trellis, soft_demod_LLR,
    //       LVA_L);
    //     } else {
    //       decode_word_LVA = hard_serial_lva(trellis, demod_word, LVA_L);
    //     }
    //   }
    //   return decode_word_LVA[0];
    // } else {
    if (is_soft) {
        decode_word = soft_viterbi_decode(trellis, soft_demod_LLR);
    } else {
        decode_word = hard_viterbi_decode(trellis, demod_word);
    }
    // }
    return decode_word;
}

std::vector<int> Comm_system_config::CRC_code() {
    information_word_for_crc = information_word;

    std::vector<int> result = information_word;

    switch (CRC_polynomial) {
    case 8: {
        CRC8 CRC_;
        if (CRC_block == 0) {
            result = CRC_.encodeCRC(result);
        } else {
            result = CRC_.encodeBlocks(result, CRC_block);
        }
        break;
    }
    case 16: {
        CRC16 CRC_;
        if (CRC_block == 0) {
            result = CRC_.encodeCRC(result);
        } else {
            result = CRC_.encodeBlocks(result, CRC_block);
        }
        break;
    }
    case 24: {
        CRC24 CRC_;
        if (CRC_block == 0) {
            result = CRC_.encodeCRC(result);
        } else {
            result = CRC_.encodeBlocks(result, CRC_block);
        }
        break;
    }
    }

    return result;
}

void Comm_system_config::run() {
    std::ofstream log_file(output_file, std::ios::app);
    log_file.sync_with_stdio(false);
    std::ofstream *ber_file = nullptr;

    if (write_output_BER_FER_file) {
        std::string ber_path =
            output_file.substr(0, output_file.find_last_of('.')) + "_BER.txt";
        output_file_BER_FER = ber_path;
        ber_file = new std::ofstream(ber_path);
        ber_file->sync_with_stdio(false);
    }

    if (!log_file.is_open()) {
        std::cerr << "Error: could not open log file " << output_file
                  << std::endl;
        return;
    }

    log_file << "Simulation" << std::endl;
    log_file << "Mode: " << mode << ", Channel: " << channel
             << ", Modulation: " << modulation << ", Decode: " << decode_mode
             << std::endl;
    log_file << "CRC: " << CRC_polynomial << ", k = " << k << ", Generators: {";
    for (std::size_t i = 0; i < trellis_generators.size(); ++i) {
        log_file << trellis_generators[i];
        if (i + 1 < trellis_generators.size()) {
            log_file << ", ";
        }
    }
    log_file << "}" << std::endl;

    if (ber_file && ber_file->is_open()) {
        *ber_file << "EbN0_dB BER         FER         Attempts" << std::endl;
    }

    const bool use_soft = (decode_mode.find("soft") != std::string::npos);
    const bool use_crc = (mode == 5);
    const bool need_modulate = (mode >= 1);
    const bool need_noise = (mode >= 2);
    const bool need_decode = (mode >= 3);
    const bool debug_mode = (mode <= 3);
    const bool info_random = (information_word_mode == 1);

    std::size_t num_points = EbN0_dB.size();

    for (std::size_t idx = 0; idx < num_points; ++idx) {
        int num_attempts;
        double current_EbN0 = EbN0_dB[idx];

        if (simulation_mode) {
            num_attempts = attempts[idx];
        } else {
            num_attempts = attempts[0];
        }

        int bit_errors = 0;
        int frame_errors = 0;
        int total_bits = 0;

        for (int attempt = 0; attempt < num_attempts; ++attempt) {
            if (info_random) {
                generate_information_word();
            }

            coding();

            if (need_modulate) {
                modulating();
            }

            if (need_noise) {
                noise(current_EbN0);
                if (use_soft) {
                    soft_demod(current_EbN0);
                } else {
                    hard_demod();
                }
            }

            if (need_decode) {
                decode();
            }

            if (debug_mode) {
                if (attempt == 0) {
                    log_file << "Information word\n";
                    print_vector(information_word, log_file);

                    log_file << "Code word\n";
                    print_vector(code_word, log_file);

                    if (need_modulate) {
                        log_file << "Modulation signal\n";
                        print_complex_vector_compact(modulation_signal,
                                                     log_file);
                    }

                    if (need_noise) {
                        log_file << "Noised signal\n";
                        print_complex_vector_compact(noised_modulation_signal,
                                                     log_file);
                    }

                    if (mode == 3) {
                        log_file << "Decoded word\n";
                        print_vector(decode_word, log_file);
                    }
                }
                log_file.close();
                if (ber_file && ber_file->is_open()) {
                    ber_file->close();
                    delete ber_file;
                }
                return;
            }

            if (use_crc) {
                std::vector<int> CRC_decode;
                bool CRC_check = false;

                if (CRC_block == 0) {
                    switch (CRC_polynomial) {
                    case 8: {
                        CRC8 crc;
                        CRC_check = crc.decodeCRC(decode_word);
                        break;
                    }
                    case 16: {
                        CRC16 crc;
                        CRC_check = crc.decodeCRC(decode_word);
                        break;
                    }
                    case 24: {
                        CRC24 crc;
                        CRC_check = crc.decodeCRC(decode_word);
                        break;
                    }
                    }
                    CRC_decode.assign(decode_word.begin(),
                                      decode_word.end() - CRC_polynomial);
                } else {
                    switch (CRC_polynomial) {
                    case 8: {
                        CRC8 crc;
                        CRC_check = crc.decodeBlocks(decode_word, CRC_block,
                                                     CRC_decode);
                        break;
                    }
                    case 16: {
                        CRC16 crc;
                        CRC_check = crc.decodeBlocks(decode_word, CRC_block,
                                                     CRC_decode);
                        break;
                    }
                    case 24: {
                        CRC24 crc;
                        CRC_check = crc.decodeBlocks(decode_word, CRC_block,
                                                     CRC_decode);
                        break;
                    }
                    }
                }

                if (information_word_for_crc.size() != CRC_decode.size()) {
                    std::cout << "ERROR0_0 " << information_word_for_crc.size()
                              << " " << CRC_decode.size() << std::endl;
                }
                for (std::size_t i = 0; i < information_word_for_crc.size();
                     ++i) {
                    if (information_word_for_crc[i] != CRC_decode[i]) {
                        bit_errors++;
                    }
                }
                total_bits += static_cast<int>(information_word.size());

                if (!CRC_check) {
                    frame_errors++;
                }

            } else {
                for (std::size_t i = 0; i < information_word.size(); ++i) {
                    if (information_word[i] != decode_word[i]) {
                        bit_errors++;
                    }
                }
                total_bits += static_cast<int>(information_word.size());

                if (!std::equal(information_word.begin(),
                                information_word.end(), decode_word.begin())) {
                    frame_errors++;
                }
            }
        }

        double BER = static_cast<double>(bit_errors) / total_bits;
        double FER = static_cast<double>(frame_errors) / num_attempts;
        log_file << std::fixed << std::setprecision(2)
                 << "Eb/N0 = " << current_EbN0 << " dB | "
                 << "Attempts: " << num_attempts << " | "
                 << std::setprecision(6) << "BER = " << BER
                 << " | FER = " << FER << " | bit error = " << bit_errors
                 << " | frame error = " << frame_errors
                 << " | total bits = " << total_bits << std::endl;

        if (ber_file && ber_file->is_open()) {
            *ber_file << std::fixed << std::setprecision(2) << current_EbN0
                      << "\t" << std::setprecision(6) << BER << "\t" << FER
                      << "\t" << num_attempts << std::endl;
        }
    }

    log_file.close();
    if (ber_file && ber_file->is_open()) {
        ber_file->close();
        delete ber_file;
    }

    if (draw_BER_FER) {
        draw_BER_FER_curves();
    }
}

void Comm_system_config::draw_BER_FER_curves() {
    std::filesystem::path script_path = "scripts/draw_for_config.py";

    if (!std::filesystem::exists(script_path)) {
        std::cerr << "[Error] Script not found relative to CWD: "
                  << std::filesystem::current_path() / script_path << '\n';
        return;
    }

    std::string cmd = "python3 \"" + script_path.string() + "\" \"" +
                      output_file_BER_FER + "\"";
    std::system(cmd.c_str());
}

void Comm_system_config::print_config() const {
    std::cout << "Input file:  " << input_json_file << std::endl;
    std::cout << "Output file: " << output_file << std::endl;

    std::cout << "CRC polynomial:     " << CRC_polynomial << " bits"
              << std::endl;
    std::cout << "Modulation order:   " << modulation
              << (modulation == 4    ? " (QPSK)"
                  : modulation == 16 ? " (QAM-16)"
                  : modulation == 64 ? " (QAM-64)"
                                     : " (QAM-256)")
              << std::endl;
    std::cout << "Info word length k: " << k << " bits" << std::endl;
    std::cout << "Info word source:   "
              << (information_word_mode == 0 ? "zero" : "random") << std::endl;
    std::cout << "Simulator mode:     " << mode << std::endl;

    std::cout << "Trellis generators: { ";
    for (std::size_t i = 0; i < trellis_generators.size(); ++i) {
        std::cout << trellis_generators[i];
        if (i + 1 < trellis_generators.size())
            std::cout << ", ";
    }
    std::cout << "}" << std::endl;
    std::cout << "Decode mode:        " << decode_mode << std::endl;
    if (decode_mode.find("WAVA") != std::string::npos) {
        std::cout << "WAVA max iter:      " << WAVA_max_iter << std::endl;
    }
    if (decode_mode.find("LVA") != std::string::npos) {
        std::cout << "LVA list size L:    " << LVA_L << std::endl;
    }

    std::cout << "Channel type:       " << channel << std::endl;
    std::cout << "Channel seed:   " << channel_seed << std::endl;

    std::cout << "Eb/N0 points (dB):  [";
    for (std::size_t i = 0; i < EbN0_dB.size(); ++i) {
        std::cout << EbN0_dB[i];
        if (i + 1 < EbN0_dB.size())
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "Attempts per point: [";
    for (std::size_t i = 0; i < attempts.size(); ++i) {
        std::cout << attempts[i];
        if (i + 1 < attempts.size())
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "Simulation mode:    " << simulation_mode
              << (simulation_mode == 0 ? " (uniform attempts)"
                                       : " (per-point attempts)")
              << std::endl;

    std::cout << "Write BER/FER file: "
              << (write_output_BER_FER_file ? "yes" : "no") << std::endl;
    std::cout << "Draw BER/FER plots: " << (draw_BER_FER ? "yes" : "no")
              << std::endl;
}

void Comm_system_config::validate() const {
    if (CRC_polynomial != 8 && CRC_polynomial != 16 && CRC_polynomial != 24) {
        std::cerr << "Warning: CRC_polynomial should be 8, 16, or 24 (got "
                  << CRC_polynomial << ")" << std::endl;
    }

    if (CRC_block < 0) {
        std::cerr << "Error: CRC block length must be positive" << std::endl;
    }

    if (modulation != 4 && modulation != 16 && modulation != 64 &&
        modulation != 256) {
        std::cerr << "Error: Invalid modulation order " << modulation
                  << " (must be 4, 16, 64, or 256)" << std::endl;
    }

    if (k <= 0) {
        std::cerr << "Error: Information word length k must be positive"
                  << std::endl;
    }

    if (mode < 0 || mode > 5) {
        std::cerr << "Error: Invalid simulator mode " << mode
                  << " (must be 0-5)" << std::endl;
    }

    const std::vector<std::string> valid_decode_modes = {"hard",
                                                         "soft",
                                                         "hard_WAVA",
                                                         "soft_WAVA",
                                                         "hard_parallel_LVA",
                                                         "soft_parallel_LVA",
                                                         "hard_serial_LVA",
                                                         "soft_serial_LVA"};

    if (std::find(valid_decode_modes.begin(), valid_decode_modes.end(),
                  decode_mode) == valid_decode_modes.end()) {
        std::cerr << "Warning: Unknown decode_mode '" << decode_mode << "'"
                  << std::endl;
    }

    if (channel != "AWGN" && channel != "RYLV") {
        std::cerr << "Error: Invalid channel '" << channel
                  << "' (must be 'AWGN' or 'RYLV')" << std::endl;
    }

    if (simulation_mode == 1 && !EbN0_dB.empty() && !attempts.empty() &&
        EbN0_dB.size() != attempts.size()) {
        std::cerr << "Error: In simulation_mode=1, EbN0_dB.size() ("
                  << EbN0_dB.size() << ") must equal attempts.size() ("
                  << attempts.size() << ")" << std::endl;
    }

    for (double val : EbN0_dB) {
        if (val < -20.0 || val > 30.0) {
            std::cerr << "Warning: Unusual Eb/N0 value " << val << " dB"
                      << std::endl;
        }
    }

    for (int att : attempts) {
        if (att <= 0) {
            std::cerr << "Error: Attempts count must be positive (got " << att
                      << ")" << std::endl;
        }
    }

    if (WAVA_max_iter <= 0) {
        std::cerr << "Warning: WAVA_max_iter should be positive" << std::endl;
    }
    if (LVA_L <= 0) {
        std::cerr << "Warning: LVA_L (list size) should be positive"
                  << std::endl;
    }
}