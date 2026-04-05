#pragma once
#include <cstdint>
#include <vector>
#include <cstddef>

class CRC8 {
private:
    uint16_t polynomial;
    static constexpr int CRC_BITS = 8;

    std::vector<int> getPolynomialBits() const;
    static void appendBits(std::vector<int>& dst, uint32_t value, int bit_count);

public:
    CRC8(uint16_t poly = 0b110011011);

    uint8_t calculate(const std::vector<int>& data);
    std::vector<int> encodeCRC(const std::vector<int>& data);
    bool decodeCRC(const std::vector<int>& encoded_data);

    std::vector<int> encodeBlocks(const std::vector<int>& data, std::size_t block_size);
    bool decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data);
};

class CRC16 {
private:
    uint32_t polynomial;
    static constexpr int CRC_BITS = 16;

    std::vector<int> getPolynomialBits() const;
    static void appendBits(std::vector<int>& dst, uint32_t value, int bit_count);

public:
    CRC16(uint32_t poly = 0b10001000000100001);

    uint16_t calculate(const std::vector<int>& data);
    std::vector<int> encodeCRC(const std::vector<int>& data);
    bool decodeCRC(const std::vector<int>& encoded_data);

    std::vector<int> encodeBlocks(const std::vector<int>& data, std::size_t block_size);
    bool decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data);
};

class CRC24 {
private:
    uint32_t polynomial;
    static constexpr int CRC_BITS = 24;

    std::vector<int> getPolynomialBits() const;
    static void appendBits(std::vector<int>& dst, uint32_t value, int bit_count);

public:
    CRC24(uint32_t poly = 0b1100000000000000001100011);

    uint32_t calculate(const std::vector<int>& data);
    std::vector<int> encodeCRC(const std::vector<int>& data);
    bool decodeCRC(const std::vector<int>& encoded_data);

    std::vector<int> encodeBlocks(const std::vector<int>& data, std::size_t block_size);
    bool decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data);
};
