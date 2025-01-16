#pragma once

#define INODE_BLOCKS 16 // 16 direct blocks
#define BLOCK_SIZE (1024 * 32) // 32 KiB
#define MAX_FILE_SIZE (INODE_BLOCKS * BLOCK_SIZE) // 512 KiB
#define MAX_FILE_NAME_LENGTH 63
#define DIRECTORY_SIZE 512 // BLOCK_SIZE / DirectoryEntry size (1 byte + 63 bytes)
