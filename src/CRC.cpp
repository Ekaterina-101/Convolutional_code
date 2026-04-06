#include "../include/crc.hpp"

// ==================================== CRC8 ====================================

CRC8::CRC8(uint16_t poly) : polynomial(poly) {}

std::vector<int> CRC8::getPolynomialBits() const {
    std::vector<int> bits;
    for (int i = CRC_BITS; i >= 0; --i) {
        bits.push_back((polynomial >> i) & 1);
    }
    return bits;
}

void CRC8::appendBits(std::vector<int>& dst, uint32_t value, int bit_count) {
    for (int i = bit_count - 1; i >= 0; --i) {
        dst.push_back((value >> i) & 1);
    }
}

uint8_t CRC8::calculate(const std::vector<int>& data) {
    std::vector<int> temp = data;

    for (int i = 0; i < CRC_BITS; ++i) {
        temp.push_back(0);
    }

    std::vector<int> poly_bits = getPolynomialBits();

    for (std::size_t i = 0; i + CRC_BITS < temp.size(); ++i) {
        if (temp[i] == 1) {
            for (int j = 0; j <= CRC_BITS; ++j) {
                temp[i + j] ^= poly_bits[j];
            }
        }
    }

    uint8_t remainder = 0;
    for (int i = 0; i < CRC_BITS; ++i) {
        remainder = static_cast<uint8_t>(remainder << 1);
        remainder = static_cast<uint8_t>(remainder | temp[temp.size() - CRC_BITS + i]);
    }

    return remainder;
}

std::vector<int> CRC8::encodeCRC(const std::vector<int>& data) {
    std::vector<int> encoded = data;
    uint8_t crc = calculate(data);
    appendBits(encoded, crc, CRC_BITS);
    return encoded;
}

bool CRC8::decodeCRC(const std::vector<int>& encoded_data) {
    if (encoded_data.size() < CRC_BITS) {
        return false;
    }

    std::vector<int> temp = encoded_data;
    std::vector<int> poly_bits = getPolynomialBits();

    for (std::size_t i = 0; i + CRC_BITS < temp.size(); ++i) {
        if (temp[i] == 1) {
            for (int j = 0; j <= CRC_BITS; ++j) {
                temp[i + j] ^= poly_bits[j];
            }
        }
    }

    for (std::size_t i = temp.size() - CRC_BITS; i < temp.size(); ++i) {
        if (temp[i] != 0) {
            return false;
        }
    }

    return true;
}

std::vector<int> CRC8::encodeBlocks(const std::vector<int>& data, std::size_t block_size) {
    std::vector<int> result;

    if (block_size == 0) {
        return result;
    }

    std::size_t i = 0;
    while (i < data.size()) {
        std::size_t current_block_size = block_size;
        if (i + current_block_size > data.size()) {
            current_block_size = data.size() - i;
        }

        std::vector<int> block(data.begin() + i, data.begin() + i + current_block_size);
        std::vector<int> encoded_block = encodeCRC(block);

        result.insert(result.end(), encoded_block.begin(), encoded_block.end());
        i += current_block_size;
    }

    return result;
}

bool CRC8::decodeBlocks(const std::vector<int>& encoded_data,
                        std::size_t block_size,
                        std::vector<int>& decoded_data,
                        std::vector<bool>& block_status) {
    decoded_data.clear();
    block_status.clear();

    if (block_size == 0) {
        return false;
    }

    bool all_ok = true;

    std::size_t i = 0;
    while (i < encoded_data.size()) {
        std::size_t remaining = encoded_data.size() - i;

        if (remaining <= static_cast<std::size_t>(CRC_BITS)) {
            return false;
        }

        std::size_t payload_size = block_size;
        if (remaining < block_size + CRC_BITS) {
            payload_size = remaining - CRC_BITS;
        }

        std::size_t encoded_block_size = payload_size + CRC_BITS;

        std::vector<int> block(encoded_data.begin() + i, encoded_data.begin() + i + encoded_block_size);

        bool ok = decodeCRC(block);
        block_status.push_back(ok);

        if (!ok) {
            all_ok = false;
        }

        decoded_data.insert(decoded_data.end(), block.begin(), block.begin() + payload_size);
        i += encoded_block_size;
    }

    return all_ok;
}

// ==================================== CRC16 ====================================

CRC16::CRC16(uint32_t poly) : polynomial(poly) {}

std::vector<int> CRC16::getPolynomialBits() const {
    std::vector<int> bits;
    for (int i = CRC_BITS; i >= 0; --i) {
        bits.push_back((polynomial >> i) & 1);
    }
    return bits;
}

void CRC16::appendBits(std::vector<int>& dst, uint32_t value, int bit_count) {
    for (int i = bit_count - 1; i >= 0; --i) {
        dst.push_back((value >> i) & 1);
    }
}

uint16_t CRC16::calculate(const std::vector<int>& data) {
    std::vector<int> temp = data;

    for (int i = 0; i < CRC_BITS; ++i) {
        temp.push_back(0);
    }

    std::vector<int> poly_bits = getPolynomialBits();

    for (std::size_t i = 0; i + CRC_BITS < temp.size(); ++i) {
        if (temp[i] == 1) {
            for (int j = 0; j <= CRC_BITS; ++j) {
                temp[i + j] ^= poly_bits[j];
            }
        }
    }

    uint16_t remainder = 0;
    for (int i = 0; i < CRC_BITS; ++i) {
        remainder = static_cast<uint16_t>(remainder << 1);
        remainder = static_cast<uint16_t>(remainder | temp[temp.size() - CRC_BITS + i]);
    }

    return remainder;
}

std::vector<int> CRC16::encodeCRC(const std::vector<int>& data) {
    std::vector<int> encoded = data;
    uint16_t crc = calculate(data);
    appendBits(encoded, crc, CRC_BITS);
    return encoded;
}

bool CRC16::decodeCRC(const std::vector<int>& encoded_data) {
    if (encoded_data.size() < CRC_BITS) {
        return false;
    }

    std::vector<int> temp = encoded_data;
    std::vector<int> poly_bits = getPolynomialBits();

    for (std::size_t i = 0; i + CRC_BITS < temp.size(); ++i) {
        if (temp[i] == 1) {
            for (int j = 0; j <= CRC_BITS; ++j) {
                temp[i + j] ^= poly_bits[j];
            }
        }
    }

    for (std::size_t i = temp.size() - CRC_BITS; i < temp.size(); ++i) {
        if (temp[i] != 0) {
            return false;
        }
    }

    return true;
}

std::vector<int> CRC16::encodeBlocks(const std::vector<int>& data, std::size_t block_size) {
    std::vector<int> result;

    if (block_size == 0) {
        return result;
    }

    std::size_t i = 0;
    while (i < data.size()) {
        std::size_t current_block_size = block_size;
        if (i + current_block_size > data.size()) {
            current_block_size = data.size() - i;
        }

        std::vector<int> block(data.begin() + i, data.begin() + i + current_block_size);
        std::vector<int> encoded_block = encodeCRC(block);

        result.insert(result.end(), encoded_block.begin(), encoded_block.end());
        i += current_block_size;
    }

    return result;
}

bool CRC16::decodeBlocks(const std::vector<int>& encoded_data,
                         std::size_t block_size,
                         std::vector<int>& decoded_data,
                         std::vector<bool>& block_status) {
    decoded_data.clear();
    block_status.clear();

    if (block_size == 0) {
        return false;
    }

    bool all_ok = true;

    std::size_t i = 0;
    while (i < encoded_data.size()) {
        std::size_t remaining = encoded_data.size() - i;

        if (remaining <= static_cast<std::size_t>(CRC_BITS)) {
            return false;
        }

        std::size_t payload_size = block_size;
        if (remaining < block_size + CRC_BITS) {
            payload_size = remaining - CRC_BITS;
        }

        std::size_t encoded_block_size = payload_size + CRC_BITS;

        std::vector<int> block(encoded_data.begin() + i,
                               encoded_data.begin() + i + encoded_block_size);

        bool ok = decodeCRC(block);
        block_status.push_back(ok);

        if (!ok) {
            all_ok = false;
        }

        decoded_data.insert(decoded_data.end(),
                            block.begin(),
                            block.begin() + payload_size);

        i += encoded_block_size;
    }

    return all_ok;
}

// ==================================== CRC24 ====================================

CRC24::CRC24(uint32_t poly) : polynomial(poly) {}

std::vector<int> CRC24::getPolynomialBits() const {
    std::vector<int> bits;
    for (int i = CRC_BITS; i >= 0; --i) {
        bits.push_back((polynomial >> i) & 1);
    }
    return bits;
}

void CRC24::appendBits(std::vector<int>& dst, uint32_t value, int bit_count) {
    for (int i = bit_count - 1; i >= 0; --i) {
        dst.push_back((value >> i) & 1);
    }
}

uint32_t CRC24::calculate(const std::vector<int>& data) {
    std::vector<int> temp = data;

    for (int i = 0; i < CRC_BITS; ++i) {
        temp.push_back(0);
    }

    std::vector<int> poly_bits = getPolynomialBits();

    for (std::size_t i = 0; i + CRC_BITS < temp.size(); ++i) {
        if (temp[i] == 1) {
            for (int j = 0; j <= CRC_BITS; ++j) {
                temp[i + j] ^= poly_bits[j];
            }
        }
    }

    uint32_t remainder = 0;
    for (int i = 0; i < CRC_BITS; ++i) {
        remainder = remainder << 1;
        remainder = remainder | static_cast<uint32_t>(temp[temp.size() - CRC_BITS + i]);
    }

    return remainder;
}

std::vector<int> CRC24::encodeCRC(const std::vector<int>& data) {
    std::vector<int> encoded = data;
    uint32_t crc = calculate(data);
    appendBits(encoded, crc, CRC_BITS);
    return encoded;
}

bool CRC24::decodeCRC(const std::vector<int>& encoded_data) {
    if (encoded_data.size() < CRC_BITS) {
        return false;
    }

    std::vector<int> temp = encoded_data;
    std::vector<int> poly_bits = getPolynomialBits();

    for (std::size_t i = 0; i + CRC_BITS < temp.size(); ++i) {
        if (temp[i] == 1) {
            for (int j = 0; j <= CRC_BITS; ++j) {
                temp[i + j] ^= poly_bits[j];
            }
        }
    }

    for (std::size_t i = temp.size() - CRC_BITS; i < temp.size(); ++i) {
        if (temp[i] != 0) {
            return false;
        }
    }

    return true;
}

std::vector<int> CRC24::encodeBlocks(const std::vector<int>& data, std::size_t block_size) {
    std::vector<int> result;

    if (block_size == 0) {
        return result;
    }

    std::size_t i = 0;
    while (i < data.size()) {
        std::size_t current_block_size = block_size;
        if (i + current_block_size > data.size()) {
            current_block_size = data.size() - i;
        }

        std::vector<int> block(data.begin() + i, data.begin() + i + current_block_size);
        std::vector<int> encoded_block = encodeCRC(block);

        result.insert(result.end(), encoded_block.begin(), encoded_block.end());
        i += current_block_size;
    }

    return result;
}

bool CRC24::decodeBlocks(const std::vector<int>& encoded_data,
                         std::size_t block_size,
                         std::vector<int>& decoded_data,
                         std::vector<bool>& block_status) {
    decoded_data.clear();
    block_status.clear();

    if (block_size == 0) {
        return false;
    }

    bool all_ok = true;

    std::size_t i = 0;
    while (i < encoded_data.size()) {
        std::size_t remaining = encoded_data.size() - i;

        if (remaining <= static_cast<std::size_t>(CRC_BITS)) {
            return false;
        }

        std::size_t payload_size = block_size;
        if (remaining < block_size + CRC_BITS) {
            payload_size = remaining - CRC_BITS;
        }

        std::size_t encoded_block_size = payload_size + CRC_BITS;

        std::vector<int> block(encoded_data.begin() + i,
                               encoded_data.begin() + i + encoded_block_size);

        bool ok = decodeCRC(block);
        block_status.push_back(ok);

        if (!ok) {
            all_ok = false;
        }

        decoded_data.insert(decoded_data.end(),
                            block.begin(),
                            block.begin() + payload_size);

        i += encoded_block_size;
    }

    return all_ok;
}
