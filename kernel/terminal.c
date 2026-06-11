#include "terminal.h"
#include <stdint.h>

volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static int cursor_row = 0;
static int cursor_col = 0;
static uint8_t text_color = 0x07;

void terminal_setcolor(uint8_t color) { text_color = color; }
uint8_t terminal_getcolor(void) { return text_color; }

void terminal_clear(void) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            VGA[r * COLS + c] = (uint16_t)(' ' | (text_color << 8));
    cursor_row = 0;
    cursor_col = 0;
}

void terminal_putchar(char ch) {
    if (ch == '\n') {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= ROWS) terminal_scroll();
        return;
    }
    if (ch == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            VGA[cursor_row * COLS + cursor_col] = (uint16_t)(' ' | (text_color << 8));
        }
        return;
    }
    VGA[cursor_row * COLS + cursor_col] = (uint16_t)(ch | (text_color << 8));
    cursor_col++;
    if (cursor_col >= COLS) {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= ROWS) terminal_scroll();
    }
}

void terminal_prints(const char* s) {
    while (*s) terminal_putchar(*s++);
}

void terminal_newline(void) { terminal_putchar('\n'); }

void terminal_scroll(void) {
    for (int r = 0; r < ROWS - 1; r++)
        for (int c = 0; c < COLS; c++)
            VGA[r * COLS + c] = VGA[(r + 1) * COLS + c];
    for (int c = 0; c < COLS; c++)
        VGA[(ROWS - 1) * COLS + c] = (uint16_t)(' ' | (text_color << 8));
    cursor_row = ROWS - 1;
}

void terminal_gotoxy(int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        cursor_col = x;
        cursor_row = y;
    }
}

void terminal_init(void) { terminal_clear(); }
