#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <string>
#include <algorithm>
#include "JSON.hpp"
#include "crc.hpp"
#include "modem.hpp"
#include "noise.hpp"
#include "convolution-coding.hpp"


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

void printResult(int N, std::vector<int>& input_signal, std::vector<int>& output_signal, double param) {
    int count_err_bit = 0;
    for (int i = 0; i < input_signal.size(); i++) {
        if (input_signal[i] != output_signal[i]) {
            count_err_bit++;
        }
    }
    std::cout << std::fixed << std::setprecision(1) << 
    "Eb/N0 = " << param << std::fixed << std::setprecision(N - 1) << 
    " pb = " << double(count_err_bit)/input_signal.size() << std::endl;
}

void print_vector(const std::vector<int> &vec) {
	for (size_t i = 0; i < vec.size(); ++i) {
		std::cout << vec[i];
		if (i < vec.size() - 1) {
			std::cout << " ";
		}
	}
	std::cout << std::endl;
	std::cout << std::endl;
}


// class Comm_system_config {
// private:
//     std::string input_json_file;
//     std::string output_file;
//     int CRC_polynomial;
//     int modulation;
//     std::vector<int> information_word;
//     std::vector<int> code_word;
//     std::vector<std::complex<double>> modulation_signal;
//     std::vector<std::complex<double>> noised_modulation_signal;
//     std::vector<double> soft_demod_LLR;
//     std::vector<int> demod_word;
//     std::vector<int> decode_word;
//     std::vector<uint32_t> trellis_generators;
//     std::string decode_mode;
//     std::string channel;
//     double EbN0_dB;
//     double EbN0_dB_start;
//     double EbN0_dB_end;
//     double EbN0_dB_step;
//     double p;
//     double p_start;
//     double p_end;
//     double p_step;
//     int iteration;
//     int n;
//     Trellis trellis;

//     void validate() const;

// public:
//     Comm_system_config();
//     Comm_system_config(int CRC_poly, int mod, 
//                       const std::vector<int>& info_word, 
//                       const std::vector<uint32_t>& trellis, 
//                       const std::string d_mode, 
//                       double ebN0);
//     Comm_system_config(const std::string name_input_file);

//     int get_CRC_polynomial() const;
//     void set_CRC_polynomial(int poly);

//     int get_modulation() const;
//     void set_modulation(int order);

//     const std::vector<int>& get_information_word() const;
//     void set_information_word(const std::vector<int>& word);
//     void set_information_word(std::vector<int>&& word);

//     const std::vector<int>& get_code_word() const;
//     void set_code_word(const std::vector<int>& word);
//     void set_code_word(std::vector<int>&& word);

//     double get_EbN0_dB() const;
//     void set_EbN0_dB(double val);
//     double get_p() const;
//     void set_p(double val);
//     double getEbN0_Linear() const;

//     void print_config() const;

//     std::vector<std::complex<double>>& modulating();
//     std::vector<double>& soft_demod();
//     std::vector<int>& demod();
//     std::vector<int>& coding();
//     std::vector<std::complex<double>>& AWGN();
//     std::vector<int>& decode();

//     void calculate_pe_AWGN();
// };

class Comm_system_config {
private:
    std::string input_json_file = "config_json/exmp.json";
    std::string output_file = "data/output.txt";
    int CRC_polynomial = 8;
    int modulation = 4;
    std::vector<int> information_word;
    std::vector<int> code_word;
    std::vector<std::complex<double>> modulation_signal;
    std::vector<std::complex<double>> noised_modulation_signal;
    std::vector<double> soft_demod_LLR;
    std::vector<int> demod_word;
    std::vector<int> decode_word;
    std::vector<uint32_t> trellis_generators = {7, 5};
    std::string decode_mode = "soft";
    std::string channel = "AWGN";
    double EbN0_dB = 0.0;
    double EbN0_dB_start = 0.0;
    double EbN0_dB_end = 6.0;
    double EbN0_dB_step = 0.5;
    double p = 0.0;
    double p_start = 0.0;
    double p_end = 1.0;
    double p_step = 0.5;
    int iteration = 1000;
    int n = 100;
    Trellis trellis = build_trellis({7, 5});

public:
    Comm_system_config() 
        : input_json_file("config_json/exmp.json"), output_file("data/output.txt"), CRC_polynomial(8), modulation(4), decode_mode("soft"), EbN0_dB(0.0), p(0.0), trellis_generators({7, 5}), channel("AWGN") {}

    Comm_system_config(int CRC_poly, int mod, 
                     const std::vector<int>& info_word, const std::vector<uint32_t>& trellis, const std::string d_mode, double ebN0)
        : CRC_polynomial(CRC_poly), modulation(mod), 
          information_word(info_word), trellis_generators(trellis), decode_mode(d_mode), EbN0_dB(ebN0) {
        validate();
    }

    Comm_system_config(const std::string name_input_file) {
        std::ifstream input_file(name_input_file);
        if (!input_file.is_open()) {
            std::cerr << "Error: can't open input file '" << name_input_file << "'"
                    << std::endl;
        }

        json input_json;
        try {
            input_json = json::parse(input_file);
        } catch (const json::parse_error &e) {
            std::cerr << "Error: JSON parse error at byte " << e.byte << ": "
                    << e.what() << std::endl;
        }

        if (input_json.contains("decode_mode")) {
            if (!input_json["decode_mode"].is_string()) {
                std::cerr << "decode_mode is not string" << std::endl;
            }
            decode_mode = input_json["decode_mode"].get<std::string>();
        }

        if (input_json.contains("channel")) {
            if (!input_json["channel"].is_string()) {
                std::cerr << "channel is not string" << std::endl;
            }
            channel = input_json["channel"].get<std::string>();
        }

        if (input_json.contains("output_file")) {
            if (!input_json["output_file"].is_string()) {
                std::cerr << "output_file is not string" << std::endl;
            }
            output_file = input_json["output_file"].get<std::string>();
        }

        if (input_json.contains("information_word")) {
            if (!input_json["information_word"].is_array()) {
                std::cerr << "information_word must be an array" << std::endl;
            }
			information_word = json_to_vector_int(input_json["information_word"]);
        }

        if (input_json.contains("code_word")) {
            if (!input_json["code_word"].is_array()) {
                std::cerr << "code_word must be an array" << std::endl;
            }
			code_word = json_to_vector_int(input_json["code_word"]);
        }

        if (input_json.contains("trellis_generators")) {
            if (!input_json["trellis_generators"].is_array()) {
                std::cerr << "trellis_generators must be an array" << std::endl;
            }
			trellis_generators = json_to_vector_int32(input_json["trellis_generators"]);
            trellis = build_trellis(trellis_generators);
        }

        if (input_json.contains("CRC_polynomial")) {
            if (!input_json["CRC_polynomial"].is_number_integer()) {
                std::cerr << "CRC_polynomial must be an integer" << std::endl;
            }
			CRC_polynomial = input_json["CRC_polynomial"].get<int>();
        }

        if (input_json.contains("modulation")) {
            if (!input_json["modulation"].is_number_integer()) {
                std::cerr << "modulation must be an integer" << std::endl;
            }
			modulation = input_json["modulation"].get<int>();
        }

        if (input_json.contains("n")) {
            if (!input_json["n"].is_number_integer()) {
                std::cerr << "n must be an integer" << std::endl;
            }
			n = input_json["n"].get<int>();
        }

        if (input_json.contains("iteration")) {
            if (!input_json["iteration"].is_number_integer()) {
                std::cerr << "iteration must be an integer" << std::endl;
            }
			iteration = input_json["iteration"].get<int>();
        }

        if (input_json.contains("EbN0_dB")) {
            if (!input_json["EbN0_dB"].is_number_float()) {
                std::cerr << "EbN0_dB must be a double" << std::endl;
            }
			EbN0_dB = input_json["EbN0_dB"].get<double>();
        }

        if (input_json.contains("EbN0_dB_start")) {
            if (!input_json["EbN0_dB_start"].is_number_float()) {
                std::cerr << "EbN0_dB_start must be a double" << std::endl;
            }
			EbN0_dB_start = input_json["EbN0_dB_start"].get<double>();
        }

        if (input_json.contains("EbN0_dB_end")) {
            if (!input_json["EbN0_dB_end"].is_number_float()) {
                std::cerr << "EbN0_dB_end must be a double" << std::endl;
            }
			EbN0_dB_end = input_json["EbN0_dB_end"].get<double>();
        }

        if (input_json.contains("EbN0_dB_step")) {
            if (!input_json["EbN0_dB_step"].is_number_float()) {
                std::cerr << "EbN0_dB_step must be a double" << std::endl;
            }
			EbN0_dB_step = input_json["EbN0_dB_step"].get<double>();
        }

        if (input_json.contains("p")) {
            if (!input_json["p"].is_number_float()) {
                std::cerr << "p must be a double" << std::endl;
            }
			p = input_json["p"].get<double>();
        }

        if (input_json.contains("p_start")) {
            if (!input_json["p_start"].is_number_float()) {
                std::cerr << "p_start must be a double" << std::endl;
            }
			p_start = input_json["p_start"].get<double>();
        }

        if (input_json.contains("p_end")) {
            if (!input_json["p_end"].is_number_float()) {
                std::cerr << "p_end must be a double" << std::endl;
            }
			p_end = input_json["p_end"].get<double>();
        }

        if (input_json.contains("p_step")) {
            if (!input_json["p_step"].is_number_float()) {
                std::cerr << "p_step must be a double" << std::endl;
            }
			p_step = input_json["p_step"].get<double>();
        }

        validate();

    }

    int get_CRC_polynomial() const { return CRC_polynomial; }
    void set_CRC_polynomial(int poly) { CRC_polynomial = poly; }

    int get_modulation() const { return modulation; }
    void set_modulation(int order) { 
        modulation = order;
        validate();
    }

    const std::vector<int>& get_information_word() const { return information_word; }
    void set_information_word(const std::vector<int>& word) {
        information_word = word;
        validate();
    }
    void set_information_word(std::vector<int>&& word) {
        information_word = std::move(word);
        validate();
    }

    const std::vector<int>& get_code_word() const { return code_word; }
    void set_code_word(const std::vector<int>& word) {
        code_word = word;
        validate();
    }
    void set_code_word(std::vector<int>&& word) {
        code_word = std::move(word);
        validate();
    }

    double get_EbN0_dB() const { return EbN0_dB; }
    void set_EbN0_dB(double val) {
        EbN0_dB = val;
        validate();
    }

    double get_p() const { return p; }
    void set_p(double val) {
        p = val;
        validate();
    }

    double getEbN0_Linear() const {
        return std::pow(10.0, EbN0_dB / 10.0);
    }

    void print_config() const {
        std::cout << "========================================" << std::endl;
        std::cout << "       Конфигурация Системы            " << std::endl;
        std::cout << "========================================" << std::endl;

        std::cout << "Файл конфигурации: " << input_json_file << std::endl;
        std::cout << "Файл вывода:       " << output_file << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        std::cout << "CRC Полином:       " << CRC_polynomial << std::endl;
        std::cout << "Порядок модуляции: " << modulation << std::endl;
        std::cout << "Длина блока (n):   " << n << std::endl;
        std::cout << "Режим декодирования: " << decode_mode << std::endl;
        
        std::cout << "Генераторы решетки: { ";

        for (const auto& gen : trellis_generators) {
            std::cout << gen << " ";
        }

        std::cout << "}" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        std::cout << "Тип канала:        " << channel << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        std::cout << "Eb/N0 (текущее):   " << EbN0_dB << " дБ" << std::endl;
        std::cout << "Eb/N0 (старт):     " << EbN0_dB_start << " дБ" << std::endl;
        std::cout << "Eb/N0 (конец):     " << EbN0_dB_end << " дБ" << std::endl;
        std::cout << "Eb/N0 (шаг):       " << EbN0_dB_step << " дБ" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        std::cout << "Вероятность (p):   " << p << std::endl;
        std::cout << "p (старт):         " << p_start << std::endl;
        std::cout << "p (конец):         " << p_end << std::endl;
        std::cout << "p (шаг):           " << p_step << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        std::cout << "Количество итераций: " << iteration << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        std::cout << "Размер инф. слова:       " << information_word.size() << " бит" << std::endl;
        print_vector(information_word);
        std::cout << "Размер кодового слова:   " << code_word.size() << " бит" << std::endl;
        print_vector(code_word);
        std::cout << "Размер модулир. сигнала: " << modulation_signal.size() << " символов" << std::endl;
        std::cout << "Размер зашумл. сигнала:  " << noised_modulation_signal.size() << " символов" << std::endl;
        std::cout << "Размер LLR (soft):       " << soft_demod_LLR.size() << " значений" << std::endl;
        std::cout << "Размер демод. слова:     " << demod_word.size() << " бит" << std::endl;
        std::cout << "Размер декод. слова:     " << decode_word.size() << " бит" << std::endl;
        print_vector(demod_word);
        
        std::cout << "========================================" << std::endl;
    }

    // void print_config() const {
    //     std::cout << "Конфигурация Системы" << std::endl;
    //     std::cout << "CRC Полином"<< CRC_polynomial << std::endl;
    //     std::cout << "Порядок модуляции (M): " << modulation << std::endl;
    //     std::cout << "Длина инф. слова: " << information_word.size() << " бит" << std::endl;
    //     std::cout << "Инф. слово: " << information_word.size() << " бит" << std::endl;
    //     std::cout << "Eb/N0 (дБ): " << EbN0_dB << std::endl << std::endl;
    // }

    std::vector<std::complex<double>>& modulating() {
        switch (modulation)
        {
        case 4:
            modulation_signal = QPSKmod(code_word);
            break;
        case 16:
            modulation_signal = QAM16mod(code_word);
        case 64:
            modulation_signal = QAM64mod(code_word);
        case 256:
            modulation_signal = QAM256mod(code_word);
        default:
            break;
        }
        return modulation_signal;
    }

    std::vector<double>& soft_demod() {
        switch (modulation)
        {
        case 4:
            soft_demod_LLR = QPSKdemod_LLR(noised_modulation_signal, EbN0_dB);
            break;
        case 16:
            soft_demod_LLR = QAM16demod_LLR(noised_modulation_signal, EbN0_dB);
        case 64:
            soft_demod_LLR = QAM64demod_LLR(noised_modulation_signal, EbN0_dB);
        case 256:
            soft_demod_LLR = QAM256demod_LLR(noised_modulation_signal, EbN0_dB);
        default:
            break;
        }
        return soft_demod_LLR;
    }

    std::vector<int>& demod() {
        switch (modulation)
        {
        case 4:
            demod_word = QPSKdemod(noised_modulation_signal);
            break;
        case 16:
            demod_word = QAM16demod(noised_modulation_signal);
        case 64:
            demod_word = QAM64demod(noised_modulation_signal);
        case 256:
            demod_word = QAM256demod(noised_modulation_signal);
        default:
            break;
        }
        return demod_word;
    }

    std::vector<int>& coding() {
        code_word = conv_encode(trellis, information_word);
        return code_word;
    }

    std::vector<std::complex<double>>& AWGN() {
        noised_modulation_signal = AWGN_Q(EbN0_dB, modulation_signal, 1.0/trellis_generators.size(), modulation);
        return noised_modulation_signal;
    }


    std::vector<int>& decode() {
        if (decode_mode == "soft") {
            decode_word = soft_viterbi_decode(trellis, soft_demod_LLR);        
        } else if (decode_mode == "hard") {
            decode_word = hard_viterbi_decode(trellis, demod_word);
        } else if (decode_mode == "WAVA") {

        } else if (decode_mode == "LVA") {

        }

        return decode_word;
    }

    void calculate_pe_AWGN() {
        std::ofstream outFile(output_file, std::ios::app);

        if (!outFile.is_open()) {
            std::cerr << "Error: could not open file " << output_file << std::endl;
            return;
        }

        outFile << "pe(Eb/N0) for n = " << n << std::endl;

        for (double SNR = EbN0_dB_start; SNR <= EbN0_dB_end; SNR += EbN0_dB_step) {
            EbN0_dB = SNR;
            int error_words = 0;
            for (int i = 0; i < iteration; i++) {
                information_word = generate_random_vector(n);
                auto code = coding();
                auto modem = modulating();
                auto noised = AWGN();
                if (decode_mode == "soft") {
                    auto demod_bits = soft_demod();
                } else {
                    auto demod_bits = demod();
                }
                auto decode_bits = decode();
                if (information_word != decode_bits) {
                    error_words++;
                }
            }
            outFile << "Eb/N0 = " << SNR << std::fixed << std::setprecision(log10(iteration) - 1) << " pe = " << double(error_words)/iteration << "\n";
        }
    }

    void calculate_pb_AWGN() {
        std::ofstream outFile(output_file, std::ios::app);

        if (!outFile.is_open()) {
            std::cerr << "Error: could not open file " << output_file << std::endl;
            return;
        }

        outFile << "pb(Eb/N0) for n = " << n << std::endl;

        for (double SNR = EbN0_dB_start; SNR <= EbN0_dB_end; SNR += EbN0_dB_step) {
            EbN0_dB = SNR;
            int error_bits = 0;
            for (int i = 0; i < iteration; i++) {
                information_word = generate_random_vector(n);
                auto code = coding();
                auto modem = modulating();
                auto noised = AWGN();
                if (decode_mode == "soft") {
                    auto demod_bits = soft_demod();
                } else {
                    auto demod_bits = demod();
                }
                auto decode_bits = decode();
                for (int j = 0; j < n; j++ ) {
                    if (information_word[j] != decode_word[j]) {
                        error_bits++;
                    }
                }
            }
            outFile << "Eb/N0 = " << SNR << std::fixed << std::setprecision(log10(iteration) - 1) << " pb = " << double(error_bits)/(iteration * n) << "\n";
        }
    }

private:
    void validate() const {
    if (modulation != 4 && modulation != 16 && modulation != 64 && modulation != 256) {
        std::cerr << "Invalid modulation order" << std::endl;
    }
    
    if (CRC_polynomial != 8 && CRC_polynomial != 16 && CRC_polynomial != 24) {
         std::cerr << "Invalid CRC polynomial" << std::endl;
    }

    for (std::size_t i = 0; i < information_word.size(); i++) {
        if (information_word[i] != 0 && information_word[i] != 1) {
            std::cerr << "Invalid information word" << std::endl;
        }
    }

    for (std::size_t i = 0; i < code_word.size(); i++) {
        if (code_word[i] != 0 && code_word[i] != 1) {
            std::cerr << "Invalid code word" << std::endl;
        }
    }

    if (decode_mode != "hard" && decode_mode != "soft" && decode_mode != "WAVA" && decode_mode != "LVA") {
        std::cerr << "Invalid decoder mode" << std::endl;
    }

    if (channel != "AWGN" && channel != "BSC" && channel != "RYLEY") {
        std::cerr << "Invalid channel" << std::endl;
    }

    if (p < 0.0 || p > 1.0) {
        std::cerr << "Invalid probability for BSC" << std::endl;
    }

    if (iteration <= 0) {
        std::cerr << "Invalid iteration for Pe" << std::endl;
    }

    if (n < 0) {
        std::cerr << "Invalid length information bits" << std::endl;
    }
}
};

// // --- Пример использования ---
// int main() {
//     // 1. Создаем информационное слово (например, последовательность битов)
//     std::vector<int> data = {1, 0, 1, 1, 0, 0, 1, 0};

//     // 2. Инициализируем конфигурацию
//     // CRC-16-CCITT = 0x1021, QPSK = 4, Eb/N0 = 5 дБ
//     CommSystemConfig config(0x1021, 4, data, 5.0);

//     // 3. Выводим информацию
//     config.printConfig();

//     // 4. Изменяем параметры через сеттеры
//     config.setEbN0_dB(10.5);
//     config.setModulationOrder(16); // 16-QAM

//     std::cout << "\nПосле изменений:" << std::endl;
//     std::cout << "Новый Eb/N0 (дБ): " << config.getEbN0_dB() << std::endl;
//     std::cout << "Новая модуляция: " << config.getModulationOrder() 
//               << " (" << config.getBitsPerSymbol() << " бит/симв)" << std::endl;

//     return 0;
// }