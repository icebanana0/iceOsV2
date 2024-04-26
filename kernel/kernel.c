#include "../include/string.h"
#include "../include/stdio.h"
#include "../include/video.h"
#include "../include/keyboard.h"
#include "../types.h"

#define MAX_FILES 10
#define MAX_DIRS 5
#define MAX_FILENAME_LENGTH 32
#define BLOCK_SIZE 512
#define MAX_BLOCKS 100
#define ROOT_DIR 0

typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    uint32_t size;
    uint32_t blocks[MAX_BLOCKS];
} FileEntry;

typedef struct {
    char dirname[MAX_FILENAME_LENGTH];
    uint32_t blocks[MAX_BLOCKS];
    uint8_t used;
} DirectoryEntry;

typedef struct {
    FileEntry files[MAX_FILES];
    DirectoryEntry dirs[MAX_DIRS];
    uint8_t blocks[MAX_BLOCKS][BLOCK_SIZE];
    uint8_t freeBlocks[MAX_BLOCKS];
} SimpleFileSystem;

SimpleFileSystem filesystem;
uint32_t currentDirIndex = ROOT_DIR;

void initFileSystem(SimpleFileSystem* fs) {
    for (int i = 0; i < MAX_FILES; i++) {
        fs->files[i].size = 0;
        fs->files[i].blocks[0] = 0;
    }
    for (int i = 0; i < MAX_DIRS; i++) {
        fs->dirs[i].used = 0;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {
        fs->freeBlocks[i] = 1;
    }
    strcpy(fs->dirs[ROOT_DIR].dirname, "root");
    fs->dirs[ROOT_DIR].used = 1;
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

int createFile(SimpleFileSystem* fs, const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].size == 0) {
            int blockIndex = findFreeBlock(fs);
            if (blockIndex == -1) return -1;
            strcpy(fs->files[i].filename, filename);
            fs->files[i].size = BLOCK_SIZE;
            fs->files[i].blocks[0] = blockIndex;
            return i;
        }
    }
    return -1;
}

int createDirectory(SimpleFileSystem* fs, const char* dirname) {
    for (int i = 0; i < MAX_DIRS; i++) {
        if (!fs->dirs[i].used) {
            strcpy(fs->dirs[i].dirname, dirname);
            fs->dirs[i].used = 1;
            return i;
        }
    }
    return -1;
}

int deleteFile(SimpleFileSystem* fs, const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].size != 0 && strcmp(fs->files[i].filename, filename) == 0) {
            fs->freeBlocks[fs->files[i].blocks[0]] = 1;
            fs->files[i].size = 0;
            fs->files[i].filename[0] = '\0';
            return 0;
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

void listDirectories(SimpleFileSystem* fs) {
    for (int i = 0; i < MAX_DIRS; i++) {
        if (fs->dirs[i].used) {
            printtext(fs->dirs[i].dirname, 0x0a, 0);
            printtext("\n", 0x0a, 0);
        }
    }
}

int findDirectory(SimpleFileSystem* fs, const char* dirname) {
    for (int i = 0; i < MAX_DIRS; i++) {
        if (fs->dirs[i].used && strcmp(fs->dirs[i].dirname, dirname) == 0) {
            return i;
        }
    }
    return -1;
}

void changeDirectory(SimpleFileSystem* fs, const char* dirname) {
    if (strcmp(dirname, "..") == 0 && currentDirIndex != ROOT_DIR) {
        currentDirIndex = ROOT_DIR;
        printtext("Changed to root directory.\n", 0x0a, 0);
        return;
    }

    int dirIndex = findDirectory(fs, dirname);
    if (dirIndex != -1) {
        currentDirIndex = dirIndex;
        printtext("Directory changed.\n", 0x0a, 0);
    } else {
        printtext("Directory not found.\n", 0x0a, 0);
    }
}

void printPrompt(SimpleFileSystem* fs) {
    if (currentDirIndex == ROOT_DIR) {
        printtext("root>", 0x0a, 0);
    } else if (currentDirIndex < MAX_DIRS && fs->dirs[currentDirIndex].used) {
        printtext(fs->dirs[currentDirIndex].dirname, 0x0a, 0);
        printtext(">", 0x0a, 0);
    } else {
        printtext("unknown>", 0x0a, 0);
    }
}


void main() {
    initFileSystem(&filesystem);
    clear_screen();
    while(1) {
    printPrompt(&filesystem);
        char command[99];
        char argument[MAX_FILENAME_LENGTH];

        scan(command, 99); // Scan for command input
		if(command[0] == '\0') 
		{
			continue;
		}
        // Split the command and the argument
        char *space = strchr(command, ' ');
        if (space != NULL) {
            *space = '\0';  // Terminate the command string
            strcpy(argument, space + 1); // Copy the argument
        } else {
            argument[0] = '\0'; // No argument provided
        }

        if(strcmp(command, "help") == 0) {
            printtext("Available commands:\nhelp\nclear\ncreate [filename]\ndelete [filename]\nlist\ncreatedir [dirname]\nlistdir\ncd [dirname or ..]\n", 0x0a, 0);
        } else if(strcmp(command, "clear") == 0) {
            clear_screen();
        } else if(strcmp(command, "create") == 0 && argument[0] != '\0') {
            if (createFile(&filesystem, argument) >= 0) {
                printtext("File created successfully!\n", 0x0a, 0);
            } else {
                printtext("Failed to create file. No free blocks available.\n", 0x04, 0);
            }
        } else if(strcmp(command, "delete") == 0 && argument[0] != '\0') {
            if (deleteFile(&filesystem, argument) == 0) {
                printtext("File deleted successfully!\n", 0x0a, 0);
            } else {
                printtext("File not found or could not be deleted.\n", 0x04, 0);
            }
        } else if(strcmp(command, "ls") == 0) {
            listFiles(&filesystem);
            listDirectories(&filesystem);
        } else if(strcmp(command, "cd") == 0 && argument[0] != '\0') {
            changeDirectory(&filesystem, argument);
        } else {
            printtext("Unknown command or missing argument\n", 0x04, 0);
        }
    }
}
