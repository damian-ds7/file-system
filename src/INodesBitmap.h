#pragma once
#include <bitset>
#include <cstdint>
#include <vector>

#include "constants.h"

/* Bitmap of free INodes index of a free INode is marked as true in the bitmap */
class INodesBitmap {
    std::vector<bool> bitmap;

public:
    INodesBitmap() = default;
    explicit INodesBitmap(const size_t size) : bitmap(size, true) {};

    /** Finds a free INode and marks it as used
     * @return index of the INode, -1 if no node found
     */
    uint16_t findINode();
};
