#include "../include/fileSystem.h"
#include "../include/string.h"
#include "../include/stdio.h"

void initFileSystem(SimpleFileSystem* fs) {
    for (int i = 0; i < MAX_FILES; i++) {
        fs->files[i].size = 0;
        fs->files[i].blocks[0] = 0;
        fs->files[i].parentDirIndex = ROOT_DIR;
    }
    for (int i = 0; i < MAX_DIRS; i++) {
        fs->dirs[i].used = 0;
        fs->dirs[i].parentDirIndex = ROOT_DIR;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {
        fs->freeBlocks[i] = 1;
    }
    strcpy(fs->dirs[ROOT_DIR].dirname, "root");
    fs->dirs[ROOT_DIR].used = 1;
    fs->dirs[ROOT_DIR].parentDirIndex = ROOT_DIR;
}

int findFreeBlock(SimpleFileSystem* fs) {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (fs->freeBlocks[i]) {
            fs->freeBlocks[i] = 0;
            return i;
        }
    }
    return -1;
}

