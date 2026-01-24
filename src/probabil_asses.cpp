#include <math.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

double Q(double x) {
    return (0.5 * std::erfc(x / std::sqrt(2.0)));
}

double pb_Q_AWGN(int k0, int d, std::vector<int>& w, double R, double SNR) {
    std::vector<double> pj;
    for (int j = d; j <= (d + 4); j++) {
        pj.push_back(Q(std::sqrt(2.0 * R * double(j) * (std::pow(10, SNR/10.0)))));
    }
    double p = 0.0;
    for (int i = 0; i < w.size(); i++) {
        p += double(w[i]) * pj[i];
    }
    return p/double(k0);
}

double pe_Q_AWGN(int L, int d, std::vector<int>& w, double R, double SNR) {
    std::vector<double> pj;
    for (int j = d; j <= (d + 4); j++) {
        // std::cout << "pw" << j << " = " << Q(std::sqrt(2.0 * R * double(j) * (std::pow(10, SNR/10.0)))) << "\n";
        pj.push_back(Q(std::sqrt(2.0 * R * double(j) * (std::pow(10, SNR/10.0)))));
    }
    double p = 0.0;
    for (int i = 0; i < w.size(); i++) {
        p += double(w[i]) * pj[i];
        // std::cout << "p" << i << " = " << p << "\n";
    }
    return p * double(L);
}

long long binomialCoefficient(int n, int k) {
    if (k < 0 || k > n) {
        return 0;
    }
    if (k > n - k)  {
        k = n - k;
    }
    
    long long result = 1;
    for (int i = 1; i <= k; ++i) {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}
double pw_BSC(int w, double p) {
    if (w < 0 || p < 0.0 || p > 1.0) {
        std::cout << "Ошибка: некорректные входные данные для pw" << std::endl;
        return -1.0;
    }
    
    double pw = 0.0;
    
    if (w % 2 == 1) {
        int i0 = (w + 1) / 2;
        for (int i = i0; i <= w; ++i) {
            pw += binomialCoefficient(w, i) * std::pow(p, i) * pow((1.0 - p), w - i);
        }
    } else {
        pw = 0.5 * binomialCoefficient(w, w/2.0) * std::pow(p, w/2.0) * std::pow(1.0 - p, w/2.0);
        
        int i0 = (w / 2) + 1;
        for (int i = i0; i <= w; ++i) {
            pw += binomialCoefficient(w, i) * std::pow(p, i) * std::pow(1.0 - p, w - i);
            
        }
    }
    
    return pw;
}

double p_SNR_BSC(double R, double SNR) {
    return Q(std::sqrt(2.0 * R * (std::pow(10, SNR/10.0))));
}

double pb_Q_BSC(int k0, int d, std::vector<int>& w, double R, double SNR) {
    std::vector<double> pj;
    for (int j = d; j <= (d + 4); j++) {
        pj.push_back(pw_BSC(j, p_SNR_BSC(R, SNR)));
    }
    double p = 0.0;
    for (int i = 0; i < w.size(); i++) {
        p += double(w[i]) * pj[i];
    }
    return p/double(k0);
}

double pe_Q_BSC(int L, int d, std::vector<int>& w, double R, double SNR) {
    std::vector<double> pj;
    for (int j = d; j <= (d + 4); j++) {
        // std::cout << "p = " << p_SNR_BSC(R, SNR) << " pw = " << pw_BSC(j, p_SNR_BSC(R, SNR)) << "\n";
        pj.push_back(pw_BSC(j, p_SNR_BSC(R, SNR)));
    }
    double p = 0.0;
    for (int i = 0; i < w.size(); i++) {
        p += double(w[i]) * pj[i];
        // std::cout << "pe" << i << " = " << p << "\n";
    }
    return p * double(L);
}

double pb_D0(int k0, double D0) {
    return (std::pow(D0, 5)/(std::pow(1.0 - 2.0*D0, 2)*double(k0)));
}
double pe_D0(int L, double D0) {
    return (double(L) * std::pow(D0, 5) / (1.0 - 2.0 * D0));
}

double D0_BSC(double p) {
    return 2 * std::sqrt(p * (1 - p));
}

double D0_AWGN(double SNR) {
    return std::exp(-(std::pow(10, SNR/10.0))*0.5);
}


// int main() {
//     std::ofstream outputFile("pb_teor_my.txt");
    
//     if (!outputFile.is_open()) {
//         std::cout << "Ошибка при открытии файла" << std::endl;
//         return 1;
//     }
    
//     std::vector<int> w2_5 = {1, 4, 12, 32, 80};
//     std::vector<int> w2_5_pe = {1, 2, 4, 8, 16};
//     std::vector<int> w3_6 = {2, 7, 18, 49, 130};
//     std::vector<int> w4_7 = {4, 12, 20, 72, 225};
//     std::vector<int> w5_8 = {2, 36, 32, 62, 332};
//     std::vector<int> w6_10 = {36, 0, 211, 0, 1404};
//     std::vector<int> w7_10 = {2, 22, 60, 148, 340};
//     std::vector<int> w8_12 = {33, 0, 281, 0, 2179};

    
///////////////////
    // double SNR0 = 2;
    // double SNR_step = 0.1;
    // double SNR_max = 5.5;
    // int k0 = 1;
    // int L = 500;
    // outputFile << "v = 0\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     if (pb_Q_AWGN(k0, 5, w2_5, 0.5, SNR) <= 1) {
    //         outputFile << "SNR = " << SNR << " pb = " << pb_Q_AWGN(k0, 5, w2_5, 0.5, SNR) << "\n";

    //     }
    // }
    // outputFile << "v = 1\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     if (pe_Q_AWGN(L, 5, w2_5_pe, 0.5, SNR) <= 1) {
    //         outputFile << "SNR = " << SNR << " pb = " << pe_Q_AWGN(L, 5, w2_5_pe, 0.5, SNR) << "\n";
    //     }
    // }

    // outputFile << "v = 2\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     if (pb_Q_BSC(k0, 5, w2_5, 0.5, SNR) <= 1) {
    //         outputFile << "SNR = " << SNR << " pb = " << pb_Q_BSC(k0, 5, w2_5, 0.5, SNR) << "\n";
    //     }
    // }
    // outputFile << "v = 3\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     if (pe_Q_BSC(L, 5, w2_5_pe, 0.5, SNR) <= 1) {
    //         outputFile << "SNR = " << SNR << " pb = " << pe_Q_BSC(L, 5, w2_5_pe, 0.5, SNR) << "\n";
    //     }
    // }
    // 
//////////////////////////

    // outputFile << "v = 210\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pb_D0(k0, D0_AWGN(SNR)) << "\n";
    // }
    // outputFile << "v = 211\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pe_D0(L, D0_AWGN(SNR)) << "\n";
    // }
    // outputFile << "v = 2110\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pb_D0(k0, D0_BSC(p_SNR_BSC(0.5, SNR))) << "\n";
    // }
    // outputFile << "v = 2111\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pe_D0(L, D0_BSC(p_SNR_BSC(0.5, SNR))) << "\n";
    // }
    // outputFile << "v = 3\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pb_KK(6, w3_6, 0.5, SNR) << "\n";
    // }
    // outputFile << "v = 4\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pb_KK(7, w4_7, 0.5, SNR) << "\n";
    // }
    // outputFile << "v = 5\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pb_KK(8, w5_8, 0.5, SNR) << "\n";
    // }
    // outputFile << "v = 6\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pb_KK(10, w6_10, 0.5, SNR) << "\n";
    // }
    // outputFile << "v = 7\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pb_KK(10, w7_10, 0.5, SNR) << "\n";
    // }
    // outputFile << "v = 8\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pb_KK(12, w8_12, 0.5, SNR) << "\n";
    // }
    // outputFile << "v = 8\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     outputFile << "SNR = " << SNR << " pb = " << pb_KK(12, w8_12, 0.5, SNR) << "\n";
    // }

    // outputFile << "w + D0\n";
    // for (double SNR = SNR0; SNR <= SNR_max; SNR += SNR_step) {
    //     double p = 0.0;
    //     for (int i = 0; i < w2_5.size(); i++) {
    //         p += double(w2_5[i]) * std::pow(D0_AWGN(SNR), i + 5);
    //     }
    //     outputFile << "SNR = " << SNR << " pb = " << p << "\n";
    // }

// }