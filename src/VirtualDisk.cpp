#include "VirtualDisk.h"

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <cstring>
#include <utility>

#include "Directory.h"
#include "typedefs.h"
#include "getTimestamp.h"

void VirtualDisk::createRootDirectory() {
    const uint32_t blockIdx = blocksBitmap.findBlocks(1)[0];
    const uint16_t nodeIdx = iNodesBitmap.findINode();

    const INode node = INode::directoryNode(blockIdx);

    iNodes[nodeIdx] = node;
    const auto directory = Directory(nodeIdx, nodeIdx);
    std::memcpy(blocks[blockIdx].data, &directory, sizeof(directory));
    ++superBlock.fileCount;
}

Directory VirtualDisk::getDirectory(const uint32_t iNodeIndex) const {
    Directory directory;
    const auto blockIdx = iNodes[iNodeIndex].blocks[0];
    std::memcpy(&directory, blocks[blockIdx].data, sizeof(Directory));
    return directory;
}

VirtualDisk::VirtualDisk(size_t diskSize, std::string diskName) : diskName(std::move(diskName)) {
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

    blocks = std::vector<Block>(blockCount);

    createRootDirectory();
}

void VirtualDisk::saveToFile() {
    std::ofstream file(diskName, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open file for writing");
    }
    superBlock.fileCount = 100;

    auto iNodesData = iNodesBitmap.getBitmap();
    auto blocksData = blocksBitmap.getBitmap();

    file.write(reinterpret_cast<char*>(&superBlock), sizeof(SuperBlock));
    file.write(reinterpret_cast<char*>(iNodes.data()), sizeof(INode) * iNodes.size());
    file.write(reinterpret_cast<char*>(iNodesData.data()), sizeof(uint8_t) * iNodesData.size());
    file.write(reinterpret_cast<char*>(blocksData.data()), sizeof(uint8_t) * blocksData.size());
    file.write(reinterpret_cast<char*>(blocks.data()), sizeof(Block) * blocks.size());
    file.close();
}

VirtualDisk VirtualDisk::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading");
    }

    SuperBlock superBlock{};
    file.read(reinterpret_cast<char*>(&superBlock), sizeof(SuperBlock));

    VirtualDisk virtualDisk(superBlock.diskSize, filename);
    virtualDisk.superBlock = superBlock;

    auto iNodeCount = superBlock.blockCount / INODE_BLOCKS;
    auto blockCount = superBlock.blockCount;

    auto iNodesData = virtualDisk.iNodesBitmap.getBitmap();
    auto blocksData = virtualDisk.blocksBitmap.getBitmap();

    file.read(reinterpret_cast<char*>(virtualDisk.iNodes.data()), sizeof(INode) * iNodeCount);
    file.read(reinterpret_cast<char*>(iNodesData.data()), sizeof(uint8_t) * iNodesData.size());
    file.read(reinterpret_cast<char*>(blocksData.data()), sizeof(uint8_t) * blocksData.size());
    file.read(reinterpret_cast<char*>(virtualDisk.blocks.data()), sizeof(Block) * blockCount);
    file.close();
    return virtualDisk;
}

bytesVector VirtualDisk::getBytesFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading");
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    bytesVector bytes(size);
    file.read(reinterpret_cast<char*>(bytes.data()), size);
    file.close();
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
    auto fileSize = bytes.size();
    auto fullBlockCount = fileSize / BLOCK_SIZE;

    for (size_t i = 0; i < fullBlockCount; ++i) {
        auto blockIdx = blockIndices[i];
        auto block = blocks[blockIdx].data;
        auto offset = i * BLOCK_SIZE;
        std::copy(bytes.begin() + offset, bytes.begin() + offset + BLOCK_SIZE, block);
    }

    if (fullBlockCount != blockIndices.size()) {
        auto blockIdx = blockIndices[fullBlockCount];
        auto block = blocks[blockIdx].data;
        auto offset = fullBlockCount * BLOCK_SIZE;
        auto sizeToCopy = fileSize - offset;
        std::copy(bytes.begin() + offset, bytes.begin() + offset + sizeToCopy, block);
    }

}

Directory VirtualDisk::getCurrentDirectory() const {
    return getDirectory(currentNode);
}

void VirtualDisk::updateCurrentDirectory(const Directory& directory) {
    const auto blockIdx = iNodes[currentNode].blocks[0];
    std::memcpy(&blocks[blockIdx].data, &directory, sizeof(Directory));
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

    auto currentDir = getCurrentDirectory();
    currentDir.addEntry(iNodeIndex, targetFileName);
    updateCurrentDirectory(currentDir);
}

bytesVector VirtualDisk::getFileContents(uint16_t iNodeIdx) {
    const auto node = iNodes[iNodeIdx];
    bytesVector contents(node.fileSize);
    const auto trailingBytesSize = node.fileSize % BLOCK_SIZE;
    const auto blockCount = node.fileSize / BLOCK_SIZE;
    int i = 0;

    for (; i < blockCount; ++i) {
        std::copy(blocks[node.blocks[i]].data, blocks[node.blocks[i]].data + BLOCK_SIZE,
                  contents.begin() + i * BLOCK_SIZE);
    }

    if (trailingBytesSize > 0) {
        std::copy(blocks[node.blocks[i]].data, blocks[node.blocks[i]].data + trailingBytesSize,
                  contents.begin() + i * BLOCK_SIZE);
    }

    return contents;
}

void VirtualDisk::copyToSystemDrive(const std::string& sourceFileName, const std::string& targetFileName) {
    auto currentDir = getCurrentDirectory();
    const DirectoryEntry* entry = currentDir.getEntry(sourceFileName);
    bytesVector bytes = getFileContents(entry->iNodeNumber);
    auto fileSize = iNodes[entry->iNodeNumber].fileSize;

    std::ofstream file(targetFileName, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open file for writing");
    }

    file.write(reinterpret_cast<char*>(bytes.data()), fileSize);
    file.close();
}

void VirtualDisk::makeDirectory(const std::string& name) {
    if (superBlock.fileCount == (superBlock.blockCount / INODE_BLOCKS)) {
        throw std::runtime_error("No space on drive");
    }

    auto currentDir = getCurrentDirectory();
    if (currentDir.getEntryCount() == DIRECTORY_SIZE) {
        throw std::runtime_error("No space for new entry in current directory");
    }

    const auto iNodeIdx = iNodesBitmap.findINode();
    const auto blockIdx = blocksBitmap.findBlocks(1)[0];

    const INode node = INode::directoryNode(blockIdx);
    iNodes[iNodeIdx] = node;

    currentDir.addEntry(iNodeIdx, name);
    updateCurrentDirectory(currentDir);
}

void VirtualDisk::removeDirectory(const std::string& name) {
    auto currentDir = getCurrentDirectory();
    const DirectoryEntry* entryToRemove = currentDir.getEntry(name);
    auto directory = getDirectory(entryToRemove->iNodeNumber);

    for (auto& entry : directory.entries) {
        auto filename = entry.filename;
        if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0 && strcmp(filename, "\0") != 0) {
            throw std::runtime_error("Directory is not empty");
        }
    }

    iNodesBitmap.freeINode(entryToRemove->iNodeNumber);
    blocksBitmap.freeBlocks(iNodes[entryToRemove->iNodeNumber].blocks[0]);

    currentDir.removeEntry(entryToRemove->iNodeNumber);
    updateCurrentDirectory(currentDir);
}
