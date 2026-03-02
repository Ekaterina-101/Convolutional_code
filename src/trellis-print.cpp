#include "trellis-print.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept>

static uint8_t bit_length(uint32_t x) {
    uint8_t len = 0;
    while (x != 0) {
        ++len;
        x >>= 1;
    }
    return len;
}

static std::string bits_msb_first(uint32_t value, uint8_t n) {
    std::string s;
    s.reserve(n);
    for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
        uint32_t b = (value >> i) & 1u;
        if (b == 0u) {
            s.push_back('0');
        } else {
            s.push_back('1');
        }
    }
    return s;
}

static std::string bits_exact_width(uint32_t value, uint8_t width) {
    std::string s;
    s.reserve(width);
    for (int i = static_cast<int>(width) - 1; i >= 0; --i) {
        uint32_t b = (value >> i) & 1u;
        if (b == 0u) {
            s.push_back('0');
        } else {
            s.push_back('1');
        }
    }
    return s;
}

void print_generators_info(uint8_t K, const std::vector<uint32_t>& generators_octal) {
    if (generators_octal.empty()) {
        throw std::runtime_error("Generators list is empty.");
    }

    std::vector<uint32_t> masks;
    masks.reserve(generators_octal.size());

    uint8_t computedK = 0;
    for (size_t i = 0; i < generators_octal.size(); ++i) {
        uint32_t m = octal_to_mask(generators_octal[i]);
        masks.push_back(m);

        uint8_t bl = bit_length(m);
        if (bl > computedK) {
            computedK = bl;
        }
    }

    uint8_t useK = K;
    if (useK == 0) {
        useK = computedK;
    }
    if (useK < 2) {
        throw std::runtime_error("Computed K is too small.");
    }

    std::cout << "Generators (octal -> mask/bin), K=" << static_cast<int>(useK) << "\n";
    for (size_t i = 0; i < generators_octal.size(); ++i) {
        std::cout << "  g" << i
                  << " = " << generators_octal[i] << "(oct)"
                  << " -> " << masks[i] << "(dec)"
                  << " -> " << bits_exact_width(masks[i], useK) << "(bin)\n";
    }
}

void print_trellis_k1_from_octal(uint8_t K,
                                 const std::vector<uint32_t>& generators_octal,
                                 bool print_header)
{
    if (generators_octal.empty()) {
        throw std::runtime_error("Generators list is empty.");
    }

    std::vector<uint32_t> masks;
    masks.reserve(generators_octal.size());

    uint8_t computedK = 0;
    for (size_t i = 0; i < generators_octal.size(); ++i) {
        uint32_t m = octal_to_mask(generators_octal[i]);
        masks.push_back(m);

        uint8_t bl = bit_length(m);
        if (bl > computedK) {
            computedK = bl;
        }
    }

    uint8_t useK = K;
    if (useK == 0) {
        useK = computedK;
    }

    Trellis T = build_trellis_k1(useK, masks);
    if (print_header) {
        print_generators_info(useK, generators_octal);
        std::cout << "\n";
    }

    print_trellis_k1(T, print_header);
}

void print_trellis_k1(const Trellis& T, bool print_header) {
    if (T.k != 1) {
        throw std::runtime_error("print_trellis_k1 expects k=1 trellis.");
    }

    if (print_header) {
        std::cout << "Trellis: rate 1/" << static_cast<int>(T.n)
                  << ", K=" << static_cast<int>(T.K)
                  << ", states=" << T.numStates
                  << ", inputSymbols=" << static_cast<int>(T.numInputSymbols)
                  << ", outputSymbols=" << T.numOutputSymbols
                  << "\n\n";
    }

    std::cout << std::left
              << std::setw(15)  << "state"
              << std::setw(5)  << "u"
              << std::setw(15) << "next"
              << std::setw(10) << "outSym"
              << "outBits\n";

    std::cout << "--------------------------------------------------------\n";

    uint8_t stateWidth = 0;
    if (T.K >= 2) {
        stateWidth = static_cast<uint8_t>(T.K - 1);
    }

    for (uint32_t s = 0; s < T.numStates; ++s) {
        for (uint32_t u = 0; u < T.numInputSymbols; ++u) {
            uint32_t ns = T.nextStates[s][u];
            uint32_t os = T.outputs[s][u];

            std::string sBits  = bits_exact_width(s, stateWidth);
            std::string nsBits = bits_exact_width(ns, stateWidth);
            std::string outBits = bits_msb_first(os, T.n);

            std::cout << std::left
                      << std::setw(15)  << (std::to_string(s) + "(" + sBits + ")")
                      << std::setw(5)  << u
                      << std::setw(15) << (std::to_string(ns) + "(" + nsBits + ")")
                      << std::setw(10) << os
                      << outBits
                      << "\n";
        }
    }

    std::cout << "\n";
}