#include <vector>
#include <cstdint>

struct Trellis {
    int k = 1;
    int numInputSymbols = 2;

    int n = 0;
    int numOutputSymbols = 0;

    int K = 0;
    int numStates = 0;

    std::vector<std::vector<uint32_t>> nextStates;
    std::vector<std::vector<uint32_t>> outputs;
};

Trellis build_trellis(const std::vector<uint32_t>& generators);
