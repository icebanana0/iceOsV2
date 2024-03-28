#include <stdint.h>
#include "../types.h"
#define VGA 0xb8000
#define VGA_COLS 80
#define VGA_ROWS 25

void printc(char c, uint8 color, int x, int y) {
    volatile uint16* mem = (uint16*)VGA;
    uint16 entry = (color << 8) | c;
    uint16 offset = y * VGA_COLS + x;
    mem[offset] = entry;
}

int main() {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        printc(' ', 0x00, i % VGA_COLS, i / VGA_COLS);
    }

    printc('H', 0x0F, 0, 0);
    printc('e', 0x0F, 1, 0);
    printc('l', 0x0F, 2, 0);
    printc('l', 0x0F, 3, 0);
    printc('o', 0x0F, 4, 0);

    return 0;
}
