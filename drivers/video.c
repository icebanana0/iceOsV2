#define CHAR_PER_LINE 80
#define VIDEO_MEMORY 0xb8000
#define VIDEO_CONTROL_PIN 0x3D4
#define VIDEO_DATA_PIN 0x3D5

int cursor_pos = -1;

// Read a byte from the specified port
unsigned char port_byte_read(unsigned short port) {
    unsigned char result;
    __asm__("inb %1, %0" : "=a"(result) : "dN"(port));
    return result;
}

// Write a byte to the specified port
void port_byte_write(unsigned short port, unsigned char data) {
    __asm__("outb %1, %0" : : "dN"(port), "a"(data));
}

// Retrieve the current cursor position
int get_cursor_pos() {
    if (cursor_pos == -1) {
        cursor_pos = 0;
        port_byte_write(VIDEO_CONTROL_PIN, 14);
        cursor_pos = port_byte_read(VIDEO_DATA_PIN) << 8;
        port_byte_write(VIDEO_CONTROL_PIN, 15);
        cursor_pos += port_byte_read(VIDEO_DATA_PIN);
    }
    return cursor_pos;
}

// Set the cursor to the specified position
void set_cursor_pos(int x, int y) {
    int pos = y * CHAR_PER_LINE + x;
    cursor_pos = pos;
    port_byte_write(VIDEO_CONTROL_PIN, 14);
    port_byte_write(VIDEO_DATA_PIN, pos >> 8);
    port_byte_write(VIDEO_CONTROL_PIN, 15);
    port_byte_write(VIDEO_DATA_PIN, pos);
}

// Clear the screen and reset cursor position
void clear_screen() {
    char *screen = (char *)VIDEO_MEMORY;
    for (int i = 0; i < 4000; i++) {
        *screen = 0x20; // space character
        *(screen + 1) = 0x02; // attribute byte
        screen += 2;
    }
    set_cursor_pos(0, 0);
    display_prompt();
}

// Scroll the screen up by one line
void scroll() {
    char *screen = (char *)VIDEO_MEMORY + 320; // start from line 2
    for (int i = 0; i < 3840; i++) {
        *(screen - 160) = *screen; // move line up
        screen++;
    }
}

// Move the cursor to the next position, considering end of line
void move_next_cursor() {
    int curpos = get_cursor_pos();
    int y = curpos / CHAR_PER_LINE;
    int x = curpos % CHAR_PER_LINE;
    x += 1;
    if (x == CHAR_PER_LINE) {
        y += 1;
        x = 0;
    }
    if (y == 25) { // if at bottom, scroll up
        scroll();
        x = 0;
        y -= 1;
    }
    set_cursor_pos(x, y);
}

// Print a single character at current cursor location
void printchar(char c, char colors, char toblink) {
    int curpos = get_cursor_pos() * 2;
    char *screen = (char *)VIDEO_MEMORY + curpos;
    if (c == '\n') {
        next_line();
        return;
    }
    *screen = c;
    *(screen + 1) = (c == toblink) ? (colors | 0b10000000) : colors;
    move_next_cursor();
}

// Print a string of text at current cursor location
void printtext(char *text, char colors, char toblink) {
    while (*text) {
        printchar(*text++, colors, toblink);
    }
}

// Move the cursor to the next line
void next_line() {
    int curpos = get_cursor_pos();
    int y = curpos / CHAR_PER_LINE;
    y += 1;
    if (y == 25) {
        scroll();
        y -= 1;
    }
    set_cursor_pos(0, y);
}

// Display a command prompt
void display_prompt() {
    char *prompt = ">";
    printtext(prompt, 0x07, 0); // light grey on black
}

// Remove a character at the specified cursor position
void rm_char_in_pos(int pos) {
    int y = pos / CHAR_PER_LINE;
    int x = pos % CHAR_PER_LINE;
    char *screen = (char *)(VIDEO_MEMORY + y * 160 + x * 2);
    *screen = 0x20; // space character
    *(screen + 1) = 0x02; // attribute byte
    set_cursor_pos(x, y);
}
