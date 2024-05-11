#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../types.h"

#define MAX_FILES 10
#define MAX_DIRS 5
#define MAX_FILENAME_LENGTH 32
#define BLOCK_SIZE 512
#define MAX_BLOCKS 100
#define ROOT_DIR 0
#define MAX_TEXT_SIZE 4096 // Maximum size of text file

typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    uint32_t size;
    uint32_t blocks[MAX_BLOCKS];
    char content[MAX_TEXT_SIZE];
    uint32_t parentDirIndex; // Index of the directory this file belongs to
} FileEntry;

typedef struct {
    char dirname[MAX_FILENAME_LENGTH];
    uint32_t blocks[MAX_BLOCKS];
    uint8_t used;
    uint32_t parentDirIndex; // Index of the parent directory
} DirectoryEntry;

typedef struct {
    FileEntry files[MAX_FILES];
    DirectoryEntry dirs[MAX_DIRS];
    uint8_t blocks[MAX_BLOCKS][BLOCK_SIZE];
    uint8_t freeBlocks[MAX_BLOCKS];
} SimpleFileSystem;

// Function declarations
void initFileSystem(SimpleFileSystem* fs);
int findFreeBlock(SimpleFileSystem* fs);

#endif
