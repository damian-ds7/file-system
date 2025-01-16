#include "BlocksBitmap.h"

#include <stdexcept>

std::vector<uint32_t> BlocksBitmap::findBlocks(const uint32_t count) {
    std::vector<uint32_t> blocks;
    for (int i = 0; i < bitmap.size(); i++) {
        if (bitmap[i]) {
            blocks.push_back(i);
            bitmap[i] = false;
            if (blocks.size() == count) {
                break;
            }
        }
    }

    if (blocks.size() != count) {
        throw std::runtime_error("Not enough free blocks");
    }

    return blocks;
}