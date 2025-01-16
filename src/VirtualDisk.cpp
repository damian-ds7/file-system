#include "VirtualDisk.h"

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <cstring>

#include "Directory.h"
#include "typedefs.h"
#include "getTimestamp.h"

void VirtualDisk::createRootDirectory() {
    auto node = INode::rootNode();
    auto blockIdx = blocksBitmap.findBlocks(1)[0];
    auto nodeIdx = iNodesBitmap.findINode();

    node.blocks[0] = blockIdx;
    iNodes[nodeIdx] = node;
    const auto directory = Directory(nodeIdx, nodeIdx);
    std::memcpy(blocks[blockIdx].data, &directory, sizeof(directory));
    ++superBlock.fileCount;
}

VirtualDisk::VirtualDisk(size_t diskSize) {
    const size_t iNodeCount = diskSize / MAX_FILE_SIZE;

    if (iNodeCount < 1) {
        throw std::runtime_error(
            "Disk size is too small, must be at least " + std::to_string(MAX_FILE_SIZE) + " bytes");
    }
    diskSize = iNodeCount * MAX_FILE_SIZE;

    const uint64_t blockCount = iNodeCount * INODE_BLOCKS;

    superBlock = SuperBlock();
    superBlock.diskSize = diskSize;
    superBlock.blockCount = blockCount;
    superBlock.fileCount = 0;
    superBlock.fileSystemLastModifiedTimestamp = getTimestamp();
    superBlock.freeBlocks = blockCount;

    iNodes = std::vector<INode>(iNodeCount);

    iNodesBitmap = INodesBitmap(iNodeCount);
    blocksBitmap = BlocksBitmap(iNodeCount * INODE_BLOCKS);
    iNodesBitmap.findINode();
    blocksBitmap.findBlocks(10);

    blocks = std::vector<Block>(blockCount);

    createRootDirectory();
}

void VirtualDisk::saveToFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open file for writing");
    }

    file.write(reinterpret_cast<char*>(&superBlock), sizeof(SuperBlock));
    file.write(reinterpret_cast<char*>(iNodes.data()), sizeof(INode) * iNodes.size());
    file.write(reinterpret_cast<char*>(&iNodesBitmap), sizeof(INodesBitmap));
    file.write(reinterpret_cast<char*>(&blocksBitmap), sizeof(BlocksBitmap));
    file.write(reinterpret_cast<char*>(blocks.data()), sizeof(Block) * blocks.size());
    file.close();
}

VirtualDisk VirtualDisk::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading");
    }

    VirtualDisk virtualDisk{};
    auto iNodeCount = virtualDisk.superBlock.blockCount / INODE_BLOCKS;
    auto blockCount = virtualDisk.superBlock.blockCount;

    file.read(reinterpret_cast<char*>(&virtualDisk.superBlock), sizeof(SuperBlock));
    file.read(reinterpret_cast<char*>(virtualDisk.iNodes.data()), sizeof(INode) * iNodeCount);
    file.read(reinterpret_cast<char*>(&virtualDisk.iNodesBitmap), sizeof(INodesBitmap));
    file.read(reinterpret_cast<char*>(&virtualDisk.blocksBitmap), sizeof(BlocksBitmap));
    file.read(reinterpret_cast<char*>(virtualDisk.blocks.data()), sizeof(Block) * blockCount);
    file.close();
    return virtualDisk;
}

bytesVector VirtualDisk::getBytesFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading");
    }
    const auto length = std::filesystem::file_size(filename);
    bytesVector bytes;
    file.read(reinterpret_cast<char*>(bytes.data()), static_cast<long>(length));
    return bytes;
}

uint16_t VirtualDisk::getBlockCountFromFile(const bytesVector& bytes) {
    uint16_t blockCount = bytes.size() / BLOCK_SIZE;
    if (blockCount * BLOCK_SIZE != bytes.size()) {
        ++blockCount;
    }
    return blockCount;
}

void VirtualDisk::saveDataToBlocks(const bytesVector& bytes, const std::vector<uint32_t>& blockIndices) {
    auto blockStart = bytes.begin();
    auto blockEnd = bytes.begin() + BLOCK_SIZE;
    for (const auto& block : blockIndices) {
        blockEnd = std::max(blockEnd, bytes.end());
        std::copy(blockStart, blockEnd, blocks[block].data);
        blockStart = blockEnd + 1;
        blockEnd += BLOCK_SIZE;
    }
}

void VirtualDisk::copyFromSystemDrive(const std::string& sourceFileName, const std::string& targetFileName) {
    if (sourceFileName.size() > MAX_FILE_NAME_LENGTH) {
        throw std::runtime_error("Source filename is too long");
    }

    if (!superBlock.canFitFile()) {
        throw std::runtime_error("No space for new files on drive");
    }

    const bytesVector bytes = getBytesFromFile(sourceFileName);

    if (bytes.size() > MAX_FILE_SIZE) {
        throw std::runtime_error("Source file is too big");
    }

    const uint32_t blockCount = getBlockCountFromFile(bytes);
    const uint16_t iNodeIndex = iNodesBitmap.findINode();
    const auto blocksIndices = blocksBitmap.findBlocks(blockCount);

    const INode fileNode = INode::fileINode(bytes.size(), blocksIndices);

    iNodes[iNodeIndex] = fileNode;
    saveDataToBlocks(bytes, blocksIndices);
}
