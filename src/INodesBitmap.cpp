#include "INodesBitmap.h"

uint16_t INodesBitmap::findINode() {
    uint16_t freeINode = -1;
    for (int i = 0; i < bitmap.size(); i++) {
        if (bitmap[i]) {
            freeINode = i;
            bitmap[i] = false;
            break;
        }
    }
    return freeINode;
}