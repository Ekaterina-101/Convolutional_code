#include "universal-trellis.hpp"

static inline uint8_t parity32(uint32_t x) {
    uint8_t p = 0;
    while (x != 0) {
        p ^= 1;
        x &= (x - 1);
    }
    return p;
}

uint32_t octal_to_mask(uint32_t oct) {
    uint32_t value = 0;
    uint32_t base = 1;

    while (oct != 0) {
        uint32_t digit = oct % 10;
        if (digit >= 8) {
            throw std::runtime_error("Invalid octal digit in polynomial.");
        }
        value += digit * base;
        base <<= 3;
        oct /= 10;
    }
    return value;
}

Trellis build_trellis_k1(uint8_t K, const std::vector<uint32_t>& generators) {
    if (K < 2) {
        throw std::runtime_error("K must be >= 2.");
    }
    if (generators.empty()) {
        throw std::runtime_error("Generators list is empty.");
    }

    Trellis T;
    T.k = 1;
    T.n = static_cast<uint8_t>(generators.size());
    T.K = K;
    T.generators = generators;

    const uint32_t m = static_cast<uint32_t>(K - 1);
    T.numStates = 1u << m;
    T.numOutputSymbols = 1u << T.n;

    T.nextStates.assign(T.numStates, std::vector<uint32_t>(2, 0));
    T.outputs.assign(T.numStates, std::vector<uint32_t>(2, 0));

    for (uint32_t state = 0; state < T.numStates; ++state) {
        for (uint32_t u = 0; u < 2; ++u) {
            uint32_t shiftReg = (u << m) | state;

            uint32_t outSym = 0;
            for (uint32_t gi = 0; gi < T.n; ++gi) {
                uint8_t bit = parity32(shiftReg & T.generators[gi]);
                outSym = (outSym << 1) | static_cast<uint32_t>(bit);
            }

            uint32_t nextState = 0;
            if (m == 1) {
                nextState = u & 1u;
            } else {
                nextState = (u << (m - 1)) | (state >> 1);
            }

            T.outputs[state][u] = outSym;
            T.nextStates[state][u] = nextState;
        }
    }

    return T;
}