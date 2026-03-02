#pragma once

#include "universal-trellis.hpp"
#include <vector>
#include <stdexcept>

std::vector<int> conv_encode_k1(const Trellis& T,
                                const std::vector<int>& input_bits,
                                bool terminateToZero);