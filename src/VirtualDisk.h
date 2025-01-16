#pragma once

#include "Block.h"
#include "SuperBlock.h"
#include "INodesBitmap.h"
#include "BlocksBitmap.h"
#include "INode.h"

class VirtualDisk {
    SuperBlock superBlock{};
    std::vector<INode> iNodes;

    INodesBitmap iNodesBitmap;
    BlocksBitmap blocksBitmap;

    std::vector<Block> blocks;

    unsigned int currentNode = 0;

    void createRootDirectory();

public:
    explicit VirtualDisk(size_t diskSize);
    VirtualDisk() = default;



    /**
     * Saves current disk instance to file
     * @param filename name to save disk to
     */
    void saveToFile(const std::string& filename);

    /**
     * Create disk from file
     * @param filename name of the existing file on system drive
     * @return instance of VirtualDisk created from given file
     */
    static VirtualDisk loadFromFile(const std::string& filename);

    /**
     * Return a vector of bytes of given file
     * @param filename filename
     * @return vector of bytes
     */
    static bytesVector getBytesFromFile(const std::string& filename);

    /**
     * Get block number required to save given bytes
     * @param bytes bytes vector created from file
     * @return block number required to save the file
     */
    static uint16_t getBlockCountFromFile(const bytesVector& bytes);

    /**
     * Saves given file data to blocks
     * @param bytes vector of data converted to bytes
     * @param blockIndices indices of allocated blocks to save data to
     */
    void saveDataToBlocks(const bytesVector& bytes, const std::vector<uint32_t>& blockIndices);

    /**
     * Copy a file from system drive to virtual disk
     * @param sourceFileName name of the file to copy
     * @param targetFileName name to save file as
     */
    void copyFromSystemDrive(const std::string& sourceFileName, const std::string& targetFileName);

    /**
    * Copy a file from virtual to system drive
     * @param sourceFileName name of the file to copy
     * @param TargetFileName name to save file as
     */
    void copyToSystemDrive(const std::string& sourceFileName, const std::string& TargetFileName);

    /**
     * Create a directory in current working directory
     * @param name name of dir to create
     */
    void makeDirectory(const std::string& name);

    /**
     * Remove a directory with given name from current working directory
     * @param name name of dir to create
     */
    void removeDirectory(const std::string& name);
};
