#include "config.hpp"

int main() {
    // Comm_system_config system("config_json/exmp.json");
    // system.run();

    std::ofstream output_file("res_ray_mod.txt");

    std::random_device rd;
    std::mt19937 rng(rd());

    int num_attens = 50000;
    int n = 200;
    for (double eb = 0.0; eb < 10.0; eb += 0.2) {
        int error_bit_qpsk = 0;
        int error_bit_qam16 = 0;
        int error_bit_qam64 = 0;
        int error_bit_qam256 = 0;

        for (int i = 0; i < num_attens; i++) {
            auto inform = generate_random_vector(rng, n);

            auto inf = conv75_encode(inform);

            auto qpsk = QPSKmod(inf);
            auto qam16 = QAM16mod(inf);
            auto qam64 = QAM64mod(inf);
            auto qam256 = QAM256mod(inf);

            std::vector<double> mu_qpsk;
            std::vector<double> mu_qam16;
            std::vector<double> mu_qam64;
            std::vector<double> mu_qam256;

            auto noise_qpsk = RayleighFading_Q(eb, qpsk, 0.5, 4, mu_qpsk, 123456);
            auto noise_qam16 = RayleighFading_Q(eb, qam16, 0.5, 16, mu_qam16, 123456);
            auto noise_qam64 = RayleighFading_Q(eb, qam64, 0.5, 64, mu_qam64, 123456);
            auto noise_qam256 = RayleighFading_Q(eb, qam256, 0.5, 256, mu_qam256, 123456);

            auto dem_qpsk = QPSKdemod_LLR_Rayleigh(noise_qpsk, mu_qpsk, eb, 0.5);
            auto dem_qam16 = QAM16demod_LLR_Rayleigh(noise_qam16, mu_qam16, eb, 0.5);
            auto dem_qam64 = QAM64demod_LLR_Rayleigh(noise_qam64, mu_qam64, eb, 0.5);
            auto dem_qam256 = QAM256demod_LLR_Rayleigh(noise_qam256, mu_qam256, eb, 0.5);

            auto decode_qpsk = conv75_soft_decode(dem_qpsk);
            auto decode_qam16 = conv75_soft_decode(dem_qam16);
            auto decode_qam64 = conv75_soft_decode(dem_qam64);
            auto decode_qam256 = conv75_soft_decode(dem_qam256);

            for (int j = 0; j < n; j++) {
                if (inform[j] != decode_qpsk[j]) {
                    error_bit_qpsk++;
                }
                if (inform[j] != decode_qam16[j]) {
                    error_bit_qam16++;
                }
                if (inform[j] != decode_qam64[j]) {
                    error_bit_qam64++;
                }
                if (inform[j] != decode_qam256[j]) {
                    error_bit_qam256++;
                }
            }
        }

        output_file << "Eb/N0 = " << eb << std::endl;
        output_file << "qpsk = " << std::fixed << std::setprecision(std::log10(num_attens) - 1) << double(error_bit_qpsk) / (num_attens * n) << " ";
        output_file << "qam16 = " << std::fixed << std::setprecision(std::log10(num_attens) - 1) << double(error_bit_qam16) / (num_attens * n) << " ";
        output_file << "qam64 = " << std::fixed << std::setprecision(std::log10(num_attens) - 1) << double(error_bit_qam64) / (num_attens * n) << " ";
        output_file << "qam256 = " << std::fixed << std::setprecision(std::log10(num_attens) - 1) << double(error_bit_qam256) / (num_attens * n) << std::endl;
    }
}