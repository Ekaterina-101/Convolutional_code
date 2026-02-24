#include "convolution-coding.hpp"

std::vector<int> conv75_encode(std::vector<int>& input_bits) {
    input_bits.push_back(0);
    input_bits.push_back(0);

    trellis75 T;
    std::vector<int> output;

    uint8_t state = 0;

    for (int u : input_bits) {
        uint8_t out_symbol = T.outputs[state][u];
        uint8_t next_state = T.nextStates[state][u];

        output.push_back(out_symbol >> 1);
        output.push_back(out_symbol & 1);

        state = next_state;
    }

    return output;
}
