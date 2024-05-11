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

SimpleFileSystem filesystem;
uint32_t currentDirIndex = ROOT_DIR;

void itoa(int value, char* str, int base) {
    char* ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;

    // Check for base validity
    if (base < 2 || base > 36) { *str = '\0'; return; }

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign if necessary
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

void printnumber(int num) {
    char numStr[12];  // Enough to hold all digits of a signed 32-bit int
    itoa(num, numStr, 10);  // Convert integer to string
    printtext(numStr, 0x0a, 0);  // Assuming printtext handles strings and color
}


void initFileSystem(SimpleFileSystem* fs) {
    for (int i = 0; i < MAX_FILES; i++) {
        fs->files[i].size = 0;
        fs->files[i].blocks[0] = 0;
        fs->files[i].parentDirIndex = ROOT_DIR;
    }
    for (int i = 0; i < MAX_DIRS; i++) {
        fs->dirs[i].used = 0;
        fs->dirs[i].parentDirIndex = ROOT_DIR; // Initialize all directories under root
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

int createFile(SimpleFileSystem* fs, const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].size == 0) {
            int blockIndex = findFreeBlock(fs);
            if (blockIndex == -1) return -1;
            strcpy(fs->files[i].filename, filename);
            fs->files[i].size = BLOCK_SIZE;
            fs->files[i].blocks[0] = blockIndex;
            fs->files[i].parentDirIndex = currentDirIndex;
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
            fs->dirs[i].parentDirIndex = currentDirIndex; // Ensure this is set right when creating a directory
            printtext("Directory created with parent index: ", 0x0a, 0);
            printnumber(currentDirIndex);
            printtext("\n", 0x0a, 0);
            return i;
        }
    }
    return -1;
}


int deleteFile(SimpleFileSystem* fs, const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].size != 0 && strcmp(fs->files[i].filename, filename) == 0 && fs->files[i].parentDirIndex == currentDirIndex) {
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
        if (fs->files[i].size != 0 && fs->files[i].parentDirIndex == currentDirIndex) {
            printtext(fs->files[i].filename, 0x0a, 0);
            printtext("\n", 0x0a, 0);
        }
    }
}

void listDirectories(SimpleFileSystem* fs) {
    for (int i = 0; i < MAX_DIRS; i++) {
        if (fs->dirs[i].used && fs->dirs[i].parentDirIndex == currentDirIndex) {
            printtext(fs->dirs[i].dirname, 0x0a, 0);
            printtext("\n", 0x0a, 0);
        }
    }
}

int findDirectory(SimpleFileSystem* fs, const char* dirname) {
    for (int i = 0; i < MAX_DIRS; i++) {
        if (fs->dirs[i].used && strcmp(fs->dirs[i].dirname, dirname) == 0 && fs->dirs[i].parentDirIndex == currentDirIndex) {
            return i;
        }
    }
    return -1;
}

void changeDirectory(SimpleFileSystem* fs, const char* dirname) {
    char numStr[20];  // Buffer for numbers

    printtext("Attempting to change directory to: ", 0x0a, 0);
    printtext(dirname, 0x0a, 0);
    printtext("\n", 0x0a, 0);

    if (strcmp(dirname, "..") == 0) {
        itoa(currentDirIndex, numStr, 10);
        printtext("Current Directory Index: ", 0x0a, 0);
        printtext(numStr, 0x0a, 0);
        printtext("\nParent Directory Index: ", 0x0a, 0);
        itoa(fs->dirs[currentDirIndex].parentDirIndex, numStr, 10);
        printtext(numStr, 0x0a, 0);
        printtext("\n", 0x0a, 0);

        if (currentDirIndex != ROOT_DIR) {
            currentDirIndex = fs->dirs[currentDirIndex].parentDirIndex;
            printtext("Changed to parent directory.\n", 0x0a, 0);
        } else {
            printtext("Already at the root directory.\n", 0x0a, 0);
        }
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

void catFile(SimpleFileSystem* fs, const char* filename) {
    int fileIndex = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].size != 0 && strcmp(fs->files[i].filename, filename) == 0 && fs->files[i].parentDirIndex == currentDirIndex) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex != -1) {
        printtext(fs->files[fileIndex].content, 0x0a, 0);
    } else {
        printtext("File not found.\n", 0x04, 0);
    }
}

void editFile(SimpleFileSystem* fs, const char* filename) {
    int fileIndex = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].size != 0 && strcmp(fs->files[i].filename, filename) == 0 && fs->files[i].parentDirIndex == currentDirIndex) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex != -1) {
        printtext("Enter text (max 4096 characters):\n", 0x0a, 0);
        scan(fs->files[fileIndex].content, MAX_TEXT_SIZE);
    } else {
        int newIndex = createFile(fs, filename);
        if (newIndex != -1) {
            printtext("Enter text (max 4096 characters):\n", 0x0a, 0);
            scan(fs->files[newIndex].content, MAX_TEXT_SIZE);
        } else {
            printtext("Failed to create or edit file.\n", 0x04, 0);
        }
    }
}

void memory_set(void *ptr, int value, size_t num) {
    unsigned char *p = ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
}


void main() {
    initFileSystem(&filesystem);
    clear_screen();
    while(1) {
        printPrompt(&filesystem);
        char command[99] = {0}; // Initialize command buffer with zeros
        char argument[MAX_FILENAME_LENGTH] = {0}; // Initialize argument buffer with zeros

        scan(command, 99); // Scan for command input
        if (command[0] == '\0') {
            continue; // Skip the loop iteration if command is empty
        }

        // Split the command and the argument
        char *space = strchr(command, ' ');
        if (space != NULL) {
            *space = '\0';  // Terminate the command string
            strcpy(argument, space + 1); // Copy the argument
        } else {
            argument[0] = '\0'; // Ensure argument is empty if there's no space
        }

        // Execute commands
        if(strcmp(command, "help") == 0) {
            printtext("Available commands:\nhelp\nclear\ncat [filename]\nls\nmkdir [dirname]\ncd [dirname or ..]\nice [filename]\n", 0x0a, 0);
        } else if(strcmp(command, "clear") == 0) {
            clear_screen();
        } else if(strcmp(command, "ls") == 0) {
            listFiles(&filesystem);
            listDirectories(&filesystem);
        } else if(strcmp(command, "cd") == 0 && argument[0] != '\0') {
            changeDirectory(&filesystem, argument);
        } else if(strcmp(command, "mkdir") == 0 && argument[0] != '\0') {
            createDirectory(&filesystem, argument);
        } else if(strcmp(command, "ice") == 0 && argument[0] != '\0') {
            editFile(&filesystem, argument);
        } else if(strcmp(command, "cat") == 0 && argument[0] != '\0') {
            catFile(&filesystem, argument);
        } else {
            printtext("Unknown command or missing argument\n", 0x04, 0);
        }

        memory_set(command, 0, sizeof(command));  // Clear the command buffer using custom memory_set
        memory_set(argument, 0, sizeof(argument));  // Clear the argument buffer using custom memory_set
        
    }
}


