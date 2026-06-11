#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

#define COLS 80
#define ROWS 25

void terminal_init(void);
void terminal_clear(void);
void terminal_putchar(char c);
void terminal_prints(const char* s);
void terminal_newline(void);
void terminal_scroll(void);
void terminal_setcolor(uint8_t color);
uint8_t terminal_getcolor(void);
void terminal_gotoxy(int x, int y);

#endif

// Цвета для вывода
#define COLOR_BLACK   0x00
#define COLOR_BLUE    0x01
#define COLOR_GREEN   0x02
#define COLOR_CYAN    0x03
#define COLOR_RED     0x04
#define COLOR_MAGENTA 0x05
#define COLOR_BROWN   0x06
#define COLOR_GRAY    0x07
#define COLOR_DARK_GRAY 0x08
#define COLOR_LIGHT_BLUE 0x09
#define COLOR_LIGHT_GREEN 0x0A
#define COLOR_LIGHT_CYAN 0x0B
#define COLOR_LIGHT_RED 0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_YELLOW 0x0E
#define COLOR_WHITE  0x0F
