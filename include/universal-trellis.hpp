#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>

struct Trellis {
    uint8_t k = 1;
    uint8_t n = 0;
    uint8_t K = 0;
    uint32_t numStates = 0;

    uint8_t numInputSymbols = 2;
    uint32_t numOutputSymbols = 0;

    std::vector<uint32_t> generators;

    std::vector<std::vector<uint32_t>> nextStates;
    std::vector<std::vector<uint32_t>> outputs;
};

uint32_t octal_to_mask(uint32_t oct);

Trellis build_trellis_k1(uint8_t K, const std::vector<uint32_t>& generators);