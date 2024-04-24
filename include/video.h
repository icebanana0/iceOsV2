#ifndef VIDEO
#define VIDEO

#define CHAR_PER_LINE 80
#define VIDEO_MEMORY 0xb8000
#define VIDEO_CONTROL_PIN 0x3D4
#define VIDEO_DATA_PIN 0x3D5

extern int cursor_pos;

unsigned char port_byte_read(unsigned short port);
void port_byte_write(unsigned short port, unsigned char data);
int get_cursor_pos();
void set_cursor_pos(int x, int y);
void clear_screen();
void scroll();
void move_next_cursor();
void printchar(char c, char colors, char toblink);
void printtext(char *text, char colors, char toblink);
void next_line();
void display_prompt();
void rm_char_in_pos(int pos);

#endif
