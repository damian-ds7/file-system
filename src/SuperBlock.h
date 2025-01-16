#pragma once
#include <cstdint>


struct SuperBlock {
    uint64_t fileSystemLastModifiedTimestamp;
    uint64_t diskSize;
    uint64_t blockCount;
    uint64_t freeBlocks;
    uint64_t fileCount;

    SuperBlock() = default;

    [[nodiscard]] bool canFitFile() const;

    void setTimestampToCurrentTime();
};
