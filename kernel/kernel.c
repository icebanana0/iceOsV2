#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/video.h"
#include "../include/keyboard.h"
#include "../types.h"

void memcpy(char *source, char *dest, int nbytes) {
    for (int i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

#define MAX_FILES 10
#define MAX_FILENAME_LENGTH 32
#define BLOCK_SIZE 512
#define MAX_BLOCKS 100

typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    uint32_t size;
    uint32_t blocks[MAX_BLOCKS];
} FileEntry;

typedef struct {
    FileEntry files[MAX_FILES];
    uint8_t blocks[MAX_BLOCKS][BLOCK_SIZE];
    uint8_t freeBlocks[MAX_BLOCKS];
} SimpleFileSystem;

SimpleFileSystem filesystem;

void initFileSystem(SimpleFileSystem* fs) {
    for (int i = 0; i < MAX_FILES; i++) {
        fs->files[i].size = 0;
        for (int j = 0; j < MAX_BLOCKS; j++) {
            fs->files[i].blocks[j] = 0;
        }
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {
        fs->freeBlocks[i] = 1;
    }
}

int findFreeBlock(SimpleFileSystem* fs) {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (fs->freeBlocks[i]) {
            fs->freeBlocks[i] = 0; // Mark as used
            return i;
        }
    }
    return -1;
}

int createFile(SimpleFileSystem* fs, const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].size == 0) {
            int blockIndex = findFreeBlock(fs);
            if (blockIndex == -1) return -1; // No free block found

            strcpy(fs->files[i].filename, filename);
            fs->files[i].size = BLOCK_SIZE;
            fs->files[i].blocks[0] = blockIndex;
            return i;
        }
    }
    return -1;
}

void listFiles(SimpleFileSystem* fs) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].size != 0) {
            printtext(fs->files[i].filename, 0x0a, 0);
            printtext("\n", 0x0a, 0);
        }
    }
}


void main() {
    initFileSystem(&filesystem);
    clear_screen();
    while(1) {
        char buf[99];
        scan(buf, 99);
        if(strcmp(buf, "help") == 0) {
            printtext("commands:\nhelp-->print this\nclear-->clear screen\ncreate-->creates a file\nlist-->list files\n", 0x0a, 0);
        }
        else if(strcmp(buf, "clear") == 0) {
            clear_screen();
        }
        else if(strcmp(buf, "create") == 0) {
            char filename[MAX_FILENAME_LENGTH];
            printtext("Enter filename: ", 0x0a, 0);
            scan(filename, MAX_FILENAME_LENGTH);
            if (createFile(&filesystem, filename) >= 0) {
                printtext("File created successfully!\n", 0x0a, 0);
            } else {
                printtext("Failed to create file. No free blocks available.\n", 0x04, 0);
            }
        }
        else if(strcmp(buf, "list") == 0) {
            listFiles(&filesystem);
        }
        else {
            printtext("unknown command\n", 0x04, 0);
        }
    }
}
