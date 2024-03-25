#include "../drivers/ports.h"

void main() {
    // Example port and data/values for demonstration purposes
    uint16_t port = 0x3F8; // COM1 serial port, for example
    uint8_t data = 0xFF;
    uint16_t value = 0xFFFF;

    outb(port, data);  // Send a byte to a port
    uint8_t receivedData = inb(port); // Read a byte from a port

    outw(port, value);  // Send a word (2 bytes) to a port
    uint16_t receivedValue = inw(port); // Read a word (2 bytes) from a port
}

void printc(char c) 
{
    *(char*)0xb8000 = c;
}
