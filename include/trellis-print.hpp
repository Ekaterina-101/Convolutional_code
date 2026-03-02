#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "universal-trellis.hpp"

void print_trellis_k1_from_octal(uint8_t K,
                                 const std::vector<uint32_t>& generators_octal,
                                 bool print_header = true);

void print_trellis_k1(const Trellis& T, bool print_header = true);

void print_generators_info(uint8_t K, const std::vector<uint32_t>& generators_octal);