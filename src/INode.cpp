#include "INode.h"
#include "getTimestamp.h"

INode INode::fileINode(uint32_t fileSize, const std::vector<uint32_t>& blockIndices) {
    auto node = INode();
    node.fileSize = fileSize;
    node.type = FILE_TYPE;
    const auto now = getTimestamp();
    node.createdTimestamp = now;
    node.modifiedTimestamp = now;
    std::copy(blockIndices.begin(), blockIndices.end(), node.blocks);
    node.linksCount = 1;
    return node;
}

INode INode::directoryNode(uint32_t blockIdx) {
    auto node = INode();
    const auto now = getTimestamp();
    node.createdTimestamp = now;
    node.modifiedTimestamp = now;
    node.fileSize = BLOCK_SIZE;
    node.linksCount = 1;
    node.type = DIRECTORY_TYPE;
    node.blocks[0] = blockIdx;
    return node;
}
