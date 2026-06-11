#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_init_graphics(void);
void vga_text_mode(void);
void vga_put_pixel(int x, int y, uint8_t color);
void vga_draw_rect(int x, int y, int w, int h, uint8_t color);
void vga_draw_line(int x1, int y1, int x2, int y2, uint8_t color);
void vga_draw_char(int x, int y, char c, uint8_t color);
void vga_draw_string(int x, int y, const char* s, uint8_t color);
void vga_clear_screen(uint8_t color);
void vga_wait_vsync(void);
int vga_is_graphic(void);
int vga_get_width(void);
int vga_get_height(void);

#endif
