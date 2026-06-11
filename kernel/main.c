// kernel/main.c
#include <stdint.h>
#include "string.h"
#include "terminal.h"
#include "keyboard.h"
#include "fs.h"
#include "shell.h"

// Мультибута-заголовок
__attribute__((section(".multiboot")))
__attribute__((aligned(4)))
static const struct {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
} multiboot_header = {
    0x1BADB002,
    0x03,
    -(0x1BADB002 + 0x03)
};

// Стек
__attribute__((section(".bss")))
static uint8_t stack[32768];

__attribute__((naked))
void _start(void) {
    __asm__ volatile (
        "mov %0, %%esp\n"
        "call kmain\n"
        "cli\n1:hlt\njmp 1b\n" : : "r"(stack + sizeof(stack))
    );
}

void kmain(void) {
    terminal_init();
    terminal_clear();
    terminal_prints("Phoenix v0.01\n");
    
    fs_init();
    shell_loop();  // Запуск оболочки
}
