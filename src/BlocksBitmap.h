#pragma once
#include <cstdint>
#include <vector>

/* Bitmap of free blocks, a free block is marked as true */
class BlocksBitmap {
    std::vector<uint8_t> bitmap;

public:
    BlocksBitmap() = default;
    explicit BlocksBitmap(const size_t size) : bitmap(size, 1) {};

    [[nodiscard]] std::vector<uint8_t>& getBitmap();


    /** Finds a given number of free blocks and marks them as used
     * @return vector of found blocks
     */
    std::vector<uint32_t> findBlocks(uint32_t count);
    void freeBlocks(const std::vector<uint32_t>& indices);
    void freeBlocks(uint32_t index);
};
