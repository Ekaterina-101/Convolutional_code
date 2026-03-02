#include "conv-coder.hpp"

static inline void push_symbol_bits_msb_first(std::vector<int>& out, uint32_t sym, uint8_t n) {
    for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
        out.push_back(static_cast<int>((sym >> i) & 1u));
    }
}

std::vector<int> conv_encode_k1(const Trellis& T,
                                const std::vector<int>& input_bits,
                                bool terminateToZero) {
    std::vector<int> out;
    size_t reserve_size = input_bits.size() * static_cast<size_t>(T.n);
    if (terminateToZero) {
        reserve_size += static_cast<size_t>(T.K - 1) * static_cast<size_t>(T.n);
    }
    out.reserve(reserve_size);

    uint32_t state = 0;

    auto step = [&](int u_bit) {
        uint32_t u = static_cast<uint32_t>(u_bit);
        uint32_t sym = T.outputs[state][u];
        push_symbol_bits_msb_first(out, sym, T.n);
        state = T.nextStates[state][u];
    };

    for (size_t i = 0; i < input_bits.size(); ++i) {
        step(input_bits[i]);
    }

    if (terminateToZero) {
        for (uint8_t i = 0; i < T.K - 1; ++i) {
            step(0);
        }
    }
    return out;
}