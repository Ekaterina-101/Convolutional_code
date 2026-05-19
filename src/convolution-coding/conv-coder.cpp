#include "convolution-coding.hpp"

std::vector<int> conv_encode(const Trellis &T, std::vector<int> &input_bits) {
    for (int i = 0; i < T.K - 1; i++) {
        input_bits.push_back(0);
    }

    std::vector<int> output;
    uint32_t state = 0;

    for (int u : input_bits) {
        uint32_t out_symbol = T.outputs[state][u];
        uint32_t next_state = T.nextStates[state][u];

        for (int i = T.n - 1; i >= 0; i--) {
            output.push_back((out_symbol >> i) & 1);
        }

        state = next_state;
    }

    return output;
}
