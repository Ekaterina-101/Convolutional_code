#include <cstdint>
#include <vector>
#include <bitset>
#include <iomanip>


class CRC8 {
private:
    uint8_t polynomial;

public:
    CRC8(uint8_t poly = 0x07);
    uint8_t calculate(const std::vector<int>& data);

    std::vector<int> encodeCRC(const std::vector<int>& data);
    bool decodeCRC(const std::vector<int>& encoded_data);

    std::vector<int> encodeBlocks(const std::vector<int>& data, std::size_t block_size);
    bool decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data);
};

class CRC16 {
private:
    uint16_t polynomial;

public:
    CRC16(uint16_t poly = 0x1021);
    uint16_t calculate(const std::vector<int>& data);

    std::vector<int> encodeCRC(const std::vector<int>& data);
    bool decodeCRC(const std::vector<int>& encoded_data);

    std::vector<int> encodeBlocks(const std::vector<int>& data, std::size_t block_size);
    bool decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data);
};

class CRC24 {
private:
    uint32_t polynomial;

public:
    CRC24(uint32_t poly = 0x864CFB);
    uint32_t calculate(const std::vector<int>& data);

    std::vector<int> encodeCRC(const std::vector<int>& data);
    bool decodeCRC(const std::vector<int>& encoded_data);

    std::vector<int> encodeBlocks(const std::vector<int>& data, std::size_t block_size);
    bool decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data);
};
