#include "universal-trellis.hpp"
#include <cmath>
#include <algorithm>

static uint32_t parity(uint32_t x) {
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x &= 0xF;
    return (0x6996 >> x) & 1;
}

Trellis build_trellis(const std::vector<uint32_t>& generators) {
    Trellis T;

    T.n = generators.size();
    T.numOutputSymbols = 1 << T.n;

    uint32_t max_gen = *std::max_element(generators.begin(), generators.end());
    int memory = std::log2(max_gen);
    T.K = memory + 1;
    T.numStates = 1 << memory;

    T.nextStates.resize(T.numStates, std::vector<uint32_t>(2));
    T.outputs.resize(T.numStates, std::vector<uint32_t>(2));

    for (uint32_t state = 0; state < T.numStates; state++) {
        for (uint32_t input = 0; input < 2; input++) {
            uint32_t reg = (input << memory) | state;

            uint32_t out_symbol = 0;

            for (int i = 0; i < T.n; i++) {
                uint32_t bit = parity(reg & generators[i]);
                out_symbol = (out_symbol << 1) | bit;
            }

            uint32_t next_state = reg >> 1;

            T.nextStates[state][input] = next_state;
            T.outputs[state][input] = out_symbol;
        }
    }

    return T;
}
