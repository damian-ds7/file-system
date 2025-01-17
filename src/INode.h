#pragma once

#include <cstdint>
#include "typedefs.h"
#include "constants.h"

#define DIRECTORY_TYPE 0
#define FILE_TYPE 1

struct INode {
    int64_t createdTimestamp;
    int64_t modifiedTimestamp;
    uint32_t fileSize;
    uint32_t blocks[INODE_BLOCKS];
    uint8_t linksCount;
    uint8_t type;

    static INode fileINode(uint32_t fileSize, const std::vector<uint32_t>& blockIndices);
    static INode directoryNode(uint32_t blockIdx);
};
