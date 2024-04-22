#define CHAR_PER_LINE 80
#define VIDEO_MEMORY 0xb8000
#define VIDEO_CONTROL_PIN 0x3D4
#define VIDEO_DATA_PIN 0x3D5

int cursor_pos = -1;

// read data from port
unsigned char port_byte_read(unsigned short port) {
    unsigned char result;
    __asm__("inb %1, %0" : "=a"(result) : "dN"(port));
    return result;
}

// write data to port
void port_byte_write(unsigned short port, unsigned char data) {
    __asm__("outb %1, %0" : : "dN"(port), "a"(data));
}

// return cursor position
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

// set cursor position
void set_cursor_pos(int x, int y) {
    int pos = y * CHAR_PER_LINE + x;
    cursor_pos = pos;
    port_byte_write(VIDEO_CONTROL_PIN, 14);
    port_byte_write(VIDEO_DATA_PIN, pos >> 8);
    port_byte_write(VIDEO_CONTROL_PIN, 15);
    port_byte_write(VIDEO_DATA_PIN, pos);
}

// clear the screen
void clear_screen() {
    char *screen = (char *)VIDEO_MEMORY;
    for (int i = 0; i < 4000; i++) {
        *screen = 0x20;
        *(screen + 1) = 0x02;
        screen += 2;
    }
    set_cursor_pos(0, 0);
}

// scroll the screen
void scroll() {
    char *screen = (char *)VIDEO_MEMORY + 320;
    for (int i = 0; i < 3840; i++) {
        *(screen - 160) = *screen;
        screen++;
    }
}

// move cursor to the next position
void move_next_cursor() {
    int curpos = get_cursor_pos();
    int y = curpos / CHAR_PER_LINE;
    int x = curpos % CHAR_PER_LINE;
    x += 1;
    if (x == CHAR_PER_LINE) {
        y += 1;
        x = 0;
    }
    if (y == 25) {
        scroll();
        x = 0;
        y -= 1;
    }
    set_cursor_pos(x, y);
}

// print a character
void printchar(char c, char colors, char toblink) {
    int curpos = get_cursor_pos() * 2;
    char *screen = (char *)curpos + VIDEO_MEMORY;
    if (c == '\n') {
        next_line();
        curpos = get_cursor_pos() * 2;
        screen = (char *)curpos + VIDEO_MEMORY;
        return;
    }
    *screen = c;
    if (c == toblink) {
        *(screen + 1) = colors | 0b10000000;
    } else {
        *(screen + 1) = colors;
    }
    screen += 2;
    move_next_cursor();
}

// print a string of text
void printtext(char *text, char colors, char toblink) {
    char c = 0;
    while ((c = *text++) != '\0') {
        printchar(c, colors, toblink);
    }
}

// move to the next line
void next_line() {
    int curpos = get_cursor_pos();
    int y = curpos / CHAR_PER_LINE;
    int x = curpos % CHAR_PER_LINE;
    x = 0;
    y += 1;
    if (y == 25) {
        scroll();
        x = 0;
        y -= 1;
    }
    set_cursor_pos(x, y);
}

// remove a character at a specific position
void rm_char_in_pos(int pos) {
    int y = pos / CHAR_PER_LINE;
    int x = pos % CHAR_PER_LINE;
    int yp = (y * 160) + VIDEO_MEMORY;
    *((char *)yp + (x * 2)) = 0x00;
    *((char *)yp + (x * 2) + 1) = 0x02;
    set_cursor_pos(x, y);
}
