#include "../include/stdlib.h"
 #include "../include/string.h"
#include "../include/video.h"
 #include "../include/stdio.h"
#include "../include/keyboard.h"
#include "../types.h"

void memcpy(char *source, char *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
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
    uint32_t blocks[BLOCK_SIZE];
} FileEntry;

typedef struct {
    FileEntry files[MAX_FILES];
    uint8_t blocks[MAX_BLOCKS][BLOCK_SIZE];
    uint8_t freeBlocks[MAX_BLOCKS];
} SimpleFileSystem;

SimpleFileSystem filesystem;

void initFileSystem(SimpleFileSystem* fs) 
{
    for (int i = 0; i < MAX_FILES; i++) 
    {
        fs->files[i].size = 0;
        for (int j = 0; j < BLOCK_SIZE; j++) 
        {
            fs->files[i].blocks[j] = -1;
        }
    }
    for (int i = 0; i < MAX_BLOCKS; i++) 
    {
        fs->freeBlocks[i] = 1;
    }
}

int createFile(SimpleFileSystem* fs, const char* filename) 
{
    for (int i = 0; i < MAX_FILES; i++) 
    {
        if (fs->files[i].size == 0) 
        {
            strcpy(fs->files[i].filename, filename);
            return i;
        }
    }
    return -1;
}

int writeFile(SimpleFileSystem* fs, int fileIndex, const uint8_t* data, uint32_t size) 
{
    if (fileIndex >= 0 && fileIndex < MAX_FILES) 
    {
        int blockCount = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        int freeBlockIndex = findFreeBlock(fs, blockCount);
        if (freeBlockIndex >= 0) 
        {
            fs->files[fileIndex].size = size;
            for (int i = 0; i < blockCount; i++) 
            {
                fs->files[fileIndex].blocks[i] = freeBlockIndex + i;
                memcpy(fs->blocks[freeBlockIndex + i], data + i * BLOCK_SIZE, BLOCK_SIZE);
                fs->freeBlocks[freeBlockIndex + i] = 0;
            }
            return 0;
        }
    }
    return -1;
}

int findFreeBlock(SimpleFileSystem* fs, int blockCount) 
{
    int count = 0;
    for (int i = 0; i < MAX_BLOCKS; i++) 
    {
        if (fs->freeBlocks[i]) 
        {
            count++;
            if (count == blockCount) 
            {
                return i - count + 1;
            }
        } 
        else 
        {
            count = 0;
        }
    }
    return -1;
}

int readFile(SimpleFileSystem* fs, int fileIndex, uint8_t* buffer, uint32_t size) 
{
    if (fileIndex >= 0 && fileIndex < MAX_FILES && size <= fs->files[fileIndex].size) 
    {
        uint32_t bytesRead = 0;
        for (int i = 0; i < BLOCK_SIZE && bytesRead < size; i++) 
        {
            if (fs->files[fileIndex].blocks[i] >= 0) 
            {
                int bytesToRead = (size - bytesRead > BLOCK_SIZE) ? BLOCK_SIZE : size - bytesRead;
                memcpy(buffer + bytesRead, fs->blocks[fs->files[fileIndex].blocks[i]], bytesToRead);
                bytesRead += bytesToRead;
            }
        }
        return bytesRead;
    }
    return -1;
}

int create(SimpleFileSystem* fs, const char* filename) 
{
    for (int i = 0; i < MAX_FILES; i++) 
    {
        if (fs->files[i].size == 0) 
        {
            strcpy(fs->files[i].filename, filename);
            return i;
        }
    }
    return -1;
}


void startcalc();

void main() {

	clear_screen();
	while(1)
	{
		char buf[99];
		scan(buf,99);
		if(strcmp(buf,"help")==0)
		{
		 printtext("commands:\nhelp-->print this\nclear-->clear screen\ncreate-->creates a file\n",0x0a,0);
		}
		else if(strcmp(buf,"clear")==0)
		{
			clear_screen();
		}
		else if(strcmp(buf, "create") == 0)
        {
            char filename[MAX_FILENAME_LENGTH];
            printtext("Enter filename: ", 0x0a, 0);
            scan(filename, MAX_FILENAME_LENGTH);
            int index = create(&filesystem, filename);
            if (index >= 0) 
            {
                printtext("File created successfully!\n", 0x0a, 0);
            } 
            else 
            {
                printtext("Failed to create file. File system is full.\n", 0x04, 0);
            }
        }
		else if (strlen(buf)>0)printtext("unknown command\n",0x04,0);

	}
}
