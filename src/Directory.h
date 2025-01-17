#pragma once

#include <cstdint>
#include <string>
#include "constants.h"

struct DirectoryEntry {
    uint8_t iNodeNumber = -1;
    char filename[MAX_FILE_NAME_LENGTH] {};

    DirectoryEntry() = default;

    DirectoryEntry(uint8_t iNodeNumber, const std::string& filename);
};

struct Directory {
    DirectoryEntry entries[DIRECTORY_SIZE];

    Directory() = default;
    Directory(uint8_t parentINodeNumber, uint8_t selfINodeNumber);

    void addEntry(uint8_t iNodeNumber, const std::string& filename);

    void removeEntry(uint8_t iNodeNumber);

    [[nodiscard]] int getEntryCount() const;

    DirectoryEntry* getEntry(const std::string& filename);
};
