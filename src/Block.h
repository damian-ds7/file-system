#pragma once
#include <cstdint>

#include "constants.h"

struct Block {
    uint8_t data[BLOCK_SIZE];
};