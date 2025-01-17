#pragma once
#include <cstdint>
#include <vector>

/* Bitmap of free INodes index of a free INode is marked as true in the bitmap */
class INodesBitmap {
    std::vector<uint8_t> bitmap;

public:
    INodesBitmap() = default;

    explicit INodesBitmap(const size_t size) : bitmap(size, 1) {
    };

    [[nodiscard]] const std::vector<uint8_t>& getBitmap() const;

    /** Finds a free INode and marks it as used
     * @return index of the INode, -1 if no node found
     */
    uint16_t findINode();

    void freeINode(uint16_t inode);
};
