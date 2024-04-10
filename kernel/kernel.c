#include "../types.h"

#define VGA_COLS 80
#define VGA_ROWS 25
#define VGA ((uint16*)0xB8000)

void printc(char c, uint8 color, int x, int y) {
    volatile uint16* mem = VGA;
    uint16 entry = (color << 8) | c;
    uint16 offset = y * VGA_COLS + x;
    mem[offset] = entry;
}

void scroll() {
    volatile uint16* mem = VGA;
    for (int row = 0; row < VGA_ROWS - 1; row++) {
        for (int col = 0; col < VGA_COLS; col++) {
            mem[row * VGA_COLS + col] = mem[(row + 1) * VGA_COLS + col];
        }
    }

    uint16 blank = 0x00 | ' ';
    for (int col = 0; col < VGA_COLS; col++) {
        mem[(VGA_ROWS - 1) * VGA_COLS + col] = blank;
    }
}

void printString(const char* string, uint8 color, int x, int y) {
    int i = 0;
    while (string[i] != '\0') {
        if (x >= VGA_COLS) {
            x = 0;
            y++;
        }
        if (y >= VGA_ROWS) {
            scroll();
            y--;
        }
        printc(string[i], color, x, y);
        i++;
        x++;
    }
}

int main() {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        printc(' ', 0x00, i % VGA_COLS, i / VGA_COLS);
    }

    printString("Hello, World", 0x0F, 0, 0);
    return 0;
}
