#include "crc.hpp"

//========================================  8  ========================================

CRC8::CRC8(uint8_t poly) : polynomial(poly) {}

uint8_t CRC8::calculate(const std::vector<int>& data) {
    uint8_t crc = 0;

    for (int byte_val : data) {
        uint8_t byte = static_cast<uint8_t>(byte_val & 0xFF);
        crc ^= byte;

        for (int i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}
    
std::vector<int> CRC8::encodeCRC(const std::vector<int>& data) {
    std::vector<int> encoded = data;
    uint8_t crc_value = calculate(data);
    encoded.push_back(static_cast<int>(crc_value));

    return encoded;
}
    
bool CRC8::decodeCRC(const std::vector<int>& encoded_data) {
    std::vector<int> data(encoded_data.begin(), encoded_data.end() - 1);
    uint8_t received_crc = static_cast<uint8_t>(encoded_data.back() & 0xFF);
    uint8_t calculated_crc = calculate(data);

    bool match = (calculated_crc == received_crc);
    return match;
}

std::vector<int> CRC8::encodeBlocks(const std::vector<int>& data, std::size_t block_size) {
    std::vector<int> result;

    if (block_size == 0) {
        return result;
    }

    std::size_t i = 0;
    std::size_t n = data.size();

    while (i < n) {
        std::size_t remaining = n - i;
        std::size_t current_block_size = block_size;

        if (remaining < block_size) {
            current_block_size = remaining;
        }

        std::vector<int> block;
        block.reserve(current_block_size);

        for (std::size_t j = 0; j < current_block_size; j++) {
            block.push_back(data[i + j] & 0xFF);
        }

        uint8_t crc_value = calculate(block);

        for (std::size_t j = 0; j < current_block_size; j++) {
            result.push_back(block[j]);
        }

        result.push_back(static_cast<int>(crc_value));

        i += current_block_size;
    }

    return result;
}

bool CRC8::decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data) {
    decoded_data.clear();

    if (block_size == 0) {
        return false;
    }

    std::size_t i = 0;
    std::size_t n = encoded_data.size();
    const std::size_t crc_len = 1;

    while (i < n) {
        std::size_t remaining = n - i;

        if (remaining <= crc_len) {
            return false;
        }

        std::size_t payload_len;

        if (remaining > block_size + crc_len) {
            payload_len = block_size;
        } else {
            payload_len = remaining - crc_len;
        }

        if (payload_len == 0) {
            return false;
        }

        std::vector<int> block;
        block.reserve(payload_len);

        for (std::size_t j = 0; j < payload_len; j++) {
            block.push_back(encoded_data[i + j] & 0xFF);
        }

        uint8_t received_crc = static_cast<uint8_t>(encoded_data[i + payload_len] & 0xFF);
        uint8_t calculated_crc = calculate(block);

        if (calculated_crc != received_crc) {
            return false;
        }

        for (std::size_t j = 0; j < payload_len; j++) {
            decoded_data.push_back(block[j]);
        }

        i += payload_len + crc_len;
    }

    return true;
}

// ================================  16  =============================================

CRC16::CRC16(uint16_t poly) : polynomial(poly) {}

uint16_t CRC16::calculate(const std::vector<int>& data) {
    uint16_t crc = 0;

    for (int byte_val : data) {
        uint8_t byte = static_cast<uint8_t>(byte_val & 0xFF);
        crc ^= static_cast<uint16_t>(byte) << 8;

        for (int i = 0; i < 8; i++) {
            if ((crc & 0x8000) != 0) {
                crc = static_cast<uint16_t>((crc << 1) ^ polynomial);
            } else {
                crc = static_cast<uint16_t>(crc << 1);
            }
        }
    }

    return crc;
}

std::vector<int> CRC16::encodeCRC(const std::vector<int>& data) {
    std::vector<int> encoded = data;
    uint16_t crc_value = calculate(data);

    int crc_hi = static_cast<int>((crc_value >> 8) & 0xFF);
    int crc_lo = static_cast<int>(crc_value & 0xFF);

    encoded.push_back(crc_hi);
    encoded.push_back(crc_lo);

    return encoded;
}

bool CRC16::decodeCRC(const std::vector<int>& encoded_data) {
    if (encoded_data.size() < 2) {
        return false;
    }

    std::vector<int> data(encoded_data.begin(), encoded_data.end() - 2);

    int crc_hi_int = encoded_data[encoded_data.size() - 2] & 0xFF;
    int crc_lo_int = encoded_data[encoded_data.size() - 1] & 0xFF;

    uint16_t received_crc = static_cast<uint16_t>((crc_hi_int << 8) | crc_lo_int);
    uint16_t calculated_crc = calculate(data);

    bool match = (calculated_crc == received_crc);
    return match;
}

std::vector<int> CRC16::encodeBlocks(const std::vector<int>& data, std::size_t block_size) {
    std::vector<int> result;

    if (block_size == 0) {
        return result;
    }

    std::size_t i = 0;
    std::size_t n = data.size();

    while (i < n) {
        std::size_t remaining = n - i;
        std::size_t current_block_size = block_size;

        if (remaining < block_size) {
            current_block_size = remaining;
        }

        std::vector<int> block;
        block.reserve(current_block_size);

        for (std::size_t j = 0; j < current_block_size; j++) {
            block.push_back(data[i + j] & 0xFF);
        }

        uint16_t crc_value = calculate(block);

        int crc_hi = static_cast<int>((crc_value >> 8) & 0xFF);
        int crc_lo = static_cast<int>(crc_value & 0xFF);

        for (std::size_t j = 0; j < current_block_size; j++) {
            result.push_back(block[j]);
        }

        result.push_back(crc_hi);
        result.push_back(crc_lo);

        i += current_block_size;
    }

    return result;
}

bool CRC16::decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data) {
    decoded_data.clear();

    if (block_size == 0) {
        return false;
    }

    std::size_t i = 0;
    std::size_t n = encoded_data.size();
    const std::size_t crc_len = 2;

    while (i < n) {
        std::size_t remaining = n - i;

        if (remaining <= crc_len) {
            return false;
        }

        std::size_t payload_len;

        if (remaining > block_size + crc_len) {
            payload_len = block_size;
        } else {
            payload_len = remaining - crc_len;
        }

        if (payload_len == 0) {
            return false;
        }

        std::vector<int> block;
        block.reserve(payload_len);

        for (std::size_t j = 0; j < payload_len; j++) {
            block.push_back(encoded_data[i + j] & 0xFF);
        }

        int crc_hi_int = encoded_data[i + payload_len] & 0xFF;
        int crc_lo_int = encoded_data[i + payload_len + 1] & 0xFF;

        uint16_t received_crc =
            static_cast<uint16_t>((crc_hi_int << 8) | crc_lo_int);

        uint16_t calculated_crc = calculate(block);

        if (calculated_crc != received_crc) {
            return false;
        }

        for (std::size_t j = 0; j < payload_len; j++) {
            decoded_data.push_back(block[j]);
        }

        i += payload_len + crc_len;
    }

    return true;
}

// ======================================  24  ========================================

CRC24::CRC24(uint32_t poly) : polynomial(poly & 0xFFFFFFu) {}

uint32_t CRC24::calculate(const std::vector<int>& data) {
    uint32_t crc = 0;

    for (int byte_val : data) {
        uint8_t byte = static_cast<uint8_t>(byte_val & 0xFF);
        crc ^= static_cast<uint32_t>(byte) << 16;

        for (int i = 0; i < 8; i++) {
            if ((crc & 0x800000u) != 0) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc = crc << 1;
            }
            crc &= 0xFFFFFFu;
        }
    }

    crc &= 0xFFFFFFu;
    return crc;
}

std::vector<int> CRC24::encodeCRC(const std::vector<int>& data) {
    std::vector<int> encoded = data;
    uint32_t crc_value = calculate(data) & 0xFFFFFFu;

    int crc_b2 = static_cast<int>((crc_value >> 16) & 0xFF);
    int crc_b1 = static_cast<int>((crc_value >> 8) & 0xFF);
    int crc_b0 = static_cast<int>(crc_value & 0xFF);

    encoded.push_back(crc_b2);
    encoded.push_back(crc_b1);
    encoded.push_back(crc_b0);

    return encoded;
}

bool CRC24::decodeCRC(const std::vector<int>& encoded_data) {
    if (encoded_data.size() < 3) {
        return false;
    }

    std::vector<int> data(encoded_data.begin(), encoded_data.end() - 3);

    int crc_b2_int = encoded_data[encoded_data.size() - 3] & 0xFF;
    int crc_b1_int = encoded_data[encoded_data.size() - 2] & 0xFF;
    int crc_b0_int = encoded_data[encoded_data.size() - 1] & 0xFF;

    uint32_t received_crc =
        (static_cast<uint32_t>(crc_b2_int) << 16) |
        (static_cast<uint32_t>(crc_b1_int) << 8)  |
        static_cast<uint32_t>(crc_b0_int);

    received_crc &= 0xFFFFFFu;

    uint32_t calculated_crc = calculate(data) & 0xFFFFFFu;

    bool match = (calculated_crc == received_crc);
    return match;
}

std::vector<int> CRC24::encodeBlocks(const std::vector<int>& data, std::size_t block_size) {
    std::vector<int> result;

    if (block_size == 0) {
        return result;
    }

    std::size_t i = 0;
    std::size_t n = data.size();

    while (i < n) {
        std::size_t remaining = n - i;
        std::size_t current_block_size = block_size;

        if (remaining < block_size) {
            current_block_size = remaining;
        }

        std::vector<int> block;
        block.reserve(current_block_size);

        for (std::size_t j = 0; j < current_block_size; j++) {
            block.push_back(data[i + j] & 0xFF);
        }

        uint32_t crc_value = calculate(block) & 0xFFFFFFu;

        int crc_b2 = static_cast<int>((crc_value >> 16) & 0xFF);
        int crc_b1 = static_cast<int>((crc_value >> 8) & 0xFF);
        int crc_b0 = static_cast<int>(crc_value & 0xFF);

        for (std::size_t j = 0; j < current_block_size; j++) {
            result.push_back(block[j]);
        }

        result.push_back(crc_b2);
        result.push_back(crc_b1);
        result.push_back(crc_b0);

        i += current_block_size;
    }

    return result;
}

bool CRC24::decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data) {
    decoded_data.clear();

    if (block_size == 0) {
        return false;
    }

    std::size_t i = 0;
    std::size_t n = encoded_data.size();
    const std::size_t crc_len = 3;

    while (i < n) {
        std::size_t remaining = n - i;

        if (remaining <= crc_len) {
            return false;
        }

        std::size_t payload_len;

        if (remaining > block_size + crc_len) {
            payload_len = block_size;
        } else {
            payload_len = remaining - crc_len;
        }

        if (payload_len == 0) {
            return false;
        }

        std::vector<int> block;
        block.reserve(payload_len);

        for (std::size_t j = 0; j < payload_len; j++) {
            block.push_back(encoded_data[i + j] & 0xFF);
        }

        int crc_b2_int = encoded_data[i + payload_len]     & 0xFF;
        int crc_b1_int = encoded_data[i + payload_len + 1] & 0xFF;
        int crc_b0_int = encoded_data[i + payload_len + 2] & 0xFF;

        uint32_t received_crc =
            (static_cast<uint32_t>(crc_b2_int) << 16) |
            (static_cast<uint32_t>(crc_b1_int) << 8)  |
            static_cast<uint32_t>(crc_b0_int);

        received_crc &= 0xFFFFFFu;

        uint32_t calculated_crc = calculate(block) & 0xFFFFFFu;

        if (calculated_crc != received_crc) {
            return false;
        }

        for (std::size_t j = 0; j < payload_len; j++) {
            decoded_data.push_back(block[j]);
        }

        i += payload_len + crc_len;
    }

    return true;
}