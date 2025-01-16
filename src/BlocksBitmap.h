#pragma once
#include <bitset>
#include <cstdint>
#include <vector>

#include "constants.h"

/* Bitmap of free blocks, a free block is marked as true */
class BlocksBitmap {
    std::vector<bool> bitmap;

public:
    BlocksBitmap() = default;
    explicit BlocksBitmap(const size_t size) : bitmap(size, true) {};

    /** Finds a given number of free blocks and marks them as used
     * @return vector of found blocks
     */
    std::vector<uint32_t> findBlocks(uint32_t count);
};
