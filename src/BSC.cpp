#include "noise.hpp"

std::vector<int> BSC(double p, std::vector<int>& word) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    std::vector<int> result = word;
    
    for (std::size_t i = 0; i < result.size(); i++) {
        double random_value = dist(gen);
        
        if (random_value < p) {
            if (result[i] == 0) {
                result[i] = 1;
            } else {
                result[i] = 0;
            }
        }
    }
    
    return result;
}

std::vector<double> BSC_soft(double p, std::vector<int>& word) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    std::vector<double> result(word.size());
    
    for (std::size_t i = 0; i < word.size(); i++) {
        double random_value = dist(gen);
        if (random_value < p) {
            result[i] = std::pow(-1, word[i]) * std::log2((1.0 - p) / p);
        }
        
        
    }
    
    return result;
}