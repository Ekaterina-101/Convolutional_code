#include "noise.hpp"

std::vector<double> RayleighFading_B(double SNR, std::vector<double>& word, double R) {
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
        
        result[i] = mu * word[i] + delte * gauss(gen);
    }
    
    return result;
}
