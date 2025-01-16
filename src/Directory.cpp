#include "Directory.h"

#include <stdexcept>
#include <cstring>

DirectoryEntry::DirectoryEntry(const uint8_t iNodeNumber, const std::string& filename) {
    std::memcpy(this->filename, filename.data(), filename.size());
    this->filename[filename.size()] = '\0';
    this->iNodeNumber = iNodeNumber;
}

Directory::Directory(const uint8_t parentINodeNumber, const uint8_t selfINodeNumber) {
    const auto current = DirectoryEntry(selfINodeNumber, ".");
    const auto parent = DirectoryEntry(parentINodeNumber, "..");
    entries[0] = current;
    entries[1] = parent;
}

void Directory::addEntry(const uint8_t iNodeNumber, const std::string& filename) {
    for (auto &entry : entries) {
        if (entry.filename[0] == '\0') {
            entry = DirectoryEntry(iNodeNumber, filename);
            return;
        }
    }
    throw std::runtime_error("Directory already full");
}

void Directory::removeEntry(const uint8_t iNodeNumber) {
    for (auto &entry : entries) {
        if (entry.iNodeNumber == iNodeNumber) {
            entry = DirectoryEntry();
            return;
        }
    }
    throw std::runtime_error("Directory already full");
}