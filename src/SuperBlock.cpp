#include "SuperBlock.h"

#include "constants.h"
#include "getTimestamp.h"

bool SuperBlock::canFitFile() const {
    return fileCount < (blockCount / INODE_BLOCKS);
}

void SuperBlock::setTimestampToCurrentTime() {
    fileSystemLastModifiedTimestamp = getTimestamp();
}

