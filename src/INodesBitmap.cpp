#include "INodesBitmap.h"

const std::vector<uint8_t>& INodesBitmap::getBitmap() const {
    return bitmap;
}

uint16_t INodesBitmap::findINode() {
    uint16_t freeINode = -1;
    for (int i = 0; i < bitmap.size(); i++) {
        if (bitmap[i]) {
            freeINode = i;
            bitmap[i] = 0;
            break;
        }
    }
    return freeINode;
}

void INodesBitmap::freeINode(uint16_t inode) {
    bitmap[inode] = 1;
}
