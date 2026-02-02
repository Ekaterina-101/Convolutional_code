#include <cstdint>
#include <vector>
#include <bitset>
#include <iomanip>


class CRC8 {
private:
    uint8_t polynomial;

public:
    CRC8(int poly = 0b110011011);
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

public:
    CRC24(uint32_t poly = 0b1100000000000000001100011);
    uint32_t calculate(const std::vector<int>& data);

    std::vector<int> encodeCRC(const std::vector<int>& data);
    bool decodeCRC(const std::vector<int>& encoded_data);

    std::vector<int> encodeBlocks(const std::vector<int>& data, std::size_t block_size);
    bool decodeBlocks(const std::vector<int>& encoded_data, std::size_t block_size, std::vector<int>& decoded_data);
};
