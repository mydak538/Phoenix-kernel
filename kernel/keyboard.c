#include "keyboard.h"
#include "io.h"

static uint8_t shift_pressed = 0;

static const char keymap[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static const char keymap_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

char keyboard_getchar(void) {
    while (1) {
        uint8_t st = inb(0x64);
        if (st & 1) {
            uint8_t sc = inb(0x60);
            if (sc & 0x80) {
                sc &= 0x7F;
                if (sc == 0x2A || sc == 0x36) shift_pressed = 0;
                continue;
            }
            if (sc == 0x2A || sc == 0x36) {
                shift_pressed = 1;
                continue;
            }
            if (sc == 0x1C) return '\n';
            if (sc == 0x0E) return '\b';
            if (sc == 0x39) return ' ';
            if (sc < 128 && keymap[sc]) {
                return shift_pressed ? keymap_shift[sc] : keymap[sc];
            }
        }
    }
}

char keyboard_getch_with_arrows(void) {
    static int extended = 0;
    while (1) {
        uint8_t st = inb(0x64);
        if (st & 1) {
            uint8_t sc = inb(0x60);
            if (sc & 0x80) {
                sc &= 0x7F;
                if (sc == 0x2A || sc == 0x36) shift_pressed = 0;
                extended = 0;
                continue;
            }
            if (sc == 0x2A || sc == 0x36) {
                shift_pressed = 1;
                continue;
            }
            if (sc == 0xE0) {
                extended = 1;
                continue;
            }
            if (extended) {
                extended = 0;
                if (sc == 0x48) return 'U';
                if (sc == 0x50) return 'D';
                if (sc == 0x4B) return 'L';
                if (sc == 0x4D) return 'R';
                continue;
            }
            if (sc == 0x1C) return '\n';
            if (sc == 0x0E) return '\b';
            if (sc == 0x39) return ' ';
            if (sc < 128 && keymap[sc]) {
                return shift_pressed ? keymap_shift[sc] : keymap[sc];
            }
        }
    }
}

int keyboard_is_key_pressed(void) {
    return inb(0x64) & 1;
}
