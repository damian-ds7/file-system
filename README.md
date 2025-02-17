# Virtual File System Implementation

This repository contains an implementation of a virtual file system based on the concept described in "System plików | Koncepcja wykonania zadania".

## Overview

The virtual file system simulates a disk using the following components:

- **Superblock**
  Stores global disk metadata such as:
  - Last modification timestamp
  - Total disk size
  - Number of data blocks and free blocks
  - Total number of files

- **I-node Table**
  Each i-node represents a file or directory and includes:
  - Creation and modification timestamps
  - File size
  - Pointers to data blocks
  - Link counter
  - File type (directory or file)

- **Bitmaps**
  - **I-node Bitmap:** Marks i-nodes as free (1) or used (0).
  - **Data Blocks Bitmap:** Tracks the allocation of each data block.

- **Data Blocks**
  Fixed size blocks (32 KiB each) where file data is stored. Each i-node is allocated 16 blocks.

Additional structures include directory elements that pair an i-node with a file/directory name (up to 63 ASCII characters) and directories that occupy a single data block containing an array of these elements.

## Supported Operations

The file system implementation provides a set of operations, including:

1. **Creating a Virtual Disk**
   - Initializes a disk of at least 32 KiB and calculates the number of data blocks accordingly.

2. **Copying Files**
   - **System Disk to Virtual Disk:** Checks file size, name length, and disk capacity; allocates an i-node and data blocks; writes file data.
   - **Virtual Disk to System Disk:** Retrieves file data using i-node pointers and writes it to the system disk.

3. **Directory Management**
   - **Creating Directories:** Allocates an i-node and a data block, then adds a new directory entry.
   - **Removing Directories:** Ensures the directory is empty before deallocating the associated i-node and data block.
   - **Listing Directory Contents:** Displays the sum of file sizes, including recursive totals from subdirectories, along with free disk space.

4. **Hard Links**
   - Creates a new directory entry pointing to an existing i-node, incrementing the link counter.

5. **File Modification**
   - **Appending Data:** Adds bytes to the file, allocating additional blocks if necessary.
   - **Truncating Files:** Reduces file size and deallocates any unneeded data blocks.

6. **Disk Usage Reporting**
   - Displays both the occupied and the actual free space (factoring in available i-nodes).

## Getting Started

1. **Clone the Repository:**
    ```bash
    git clone https://github.com/damian-ds7/file-system
    ```
2. **Build executable**
    ```bash
    cd file-system
    mkdir build
    cd build
    cmake ..
    make
    ```
3. **Run file system executable**
   ```bash
   ./FileSystem
   ```

# Complete Usage Examples

## 1. Creating or loading a virtual disk

### Creating a New Disk
    Enter the name of the virtual disk: newDisk
    File does not exist. Do you want to create it? (yes/no): yes
    Enter size of the disk in bytes: 100000000
    / >

### Loading an existing disk
    Enter the name of the virtual disk: disk
    / >

## 2. Copying a file from system to virtual disk
    / > ls
    . ..
    / > import test.jpg test
    / > ls
    test         142780 B   5 blocks

## 3. Creating a directory
    / > mkdir dir
    / > ls
    test         142780 B   5 blocks
    dir          32768 B    1 blocks
    / > cd dir
    /dir >

## 4. Removing a directory

### Removing an empty directory
    /dir > cd ..
    / > rmdir dir

### Non-empty directory
    / > rmdir dir
    Directory is not empty

## 5. Copying a file from virtual disk to system
    / > ls
    test         142780 B   5 blocks
    / > export test t.jpg

## 6. Listing directories and disk usage
    / > ls
    test         142780 B   5 blocks
    dir          32768 B    1 blocks
    / > ls dir
    test2        142780 B   5 blocks
    / > dirinfo
    Total size of files in current directory: 175548 B
    / > dirinfo -r
    Total size of files in current directory and subdirectories: 351996 B

## 7. Creating a hard link
    / > ln dir/test2 link
    / > ls
    test         142780 B   5 blocks
    dir          32768 B    1 blocks
    link         142780 B   5 blocks

    / > mkdir dir/innerDir
    / > ln dir/innerDir dirLink
    / > ls
    test         142780 B   5 blocks
    dir          32768 B    1 blocks
    link         142780 B   5 blocks
    dirLink      32768 B    1 blocks
    / > cd dirLink
    /dir/innerDir >

## 8. Removing a file or link
    / > info
    Disk size: 99614720 bytes
    Free space: 99340858 bytes
    Actual free space: 96993280 bytes
    Free blocks: 3027
    Total blocks: 3040
    Files on disk: 5

    / > rm fileLink
    / > info
    Disk size: 99614720 bytes
    Free space: 99230856 bytes
    Actual free space: 96993580 bytes
    Free blocks: 3027
    Total blocks: 3040
    Files on disk: 5

    / > rm dir/test2
    / > info
    Disk size: 99614720 bytes
    Free space: 99373636 bytes
    Actual free space: 97517568 bytes
    Free blocks: 3032
    Total blocks: 3040
    Files on disk: 4

## 9. Appending given amount of bytes to a file
    /shrinkExpand > ls
    .
    ..
    file         10 B      1 blocks
    /shrinkExpand > expand file 20000
    /shrinkExpand > ls
    .
    ..
    file         20010 B   1 blocks
    /shrinkExpand > expand file 100000
    /shrinkExpand > ls
    .
    ..
    file         120010 B   4 blocks

## 10. Truncating a file
    /shrinkExpand > shrink file 20000
    /shrinkExpand > ls
    .
    ..
    file         100010 B   4 blocks
    /shrinkExpand > shrink file 100000
    /shrinkExpand > ls
    .
    ..
    file         10 B   4 blocks

## 11.
    / > info
    Disk size: 99614720 bytes
    Free space: 99340858 bytes
    Actual free space: 96468992 bytes
    Free blocks: 3030
    Total blocks: 3040
    Files on disk: 6
    / >
