#include <cstdint>
#include <vector>

typedef struct {
    uint8_t numInputSymbols = 2;
    uint8_t numOutputSymbols = 4;
    uint8_t numStates = 4;

    uint8_t nextStates[4][2] = {
        {0,2},
        {0,2},
        {1,3},
        {1,3}
    };

    uint8_t outputs[4][2] = {
        {0,3},
        {3,0},
        {2,1},
        {1,2}
    };
} trellis75;

std::vector<int> conv75_encode(std::vector<int>& input_bits);
std::vector<int> conv75_viterbi_decode(std::vector<int>& demodulated_signal);