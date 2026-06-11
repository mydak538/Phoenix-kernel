#include <stdint.h>
#include "string.h"
#include "terminal.h"

// Максимальный размер программы
#define MAX_BIN_SIZE 65536

// Структура для загруженной программы
typedef struct {
    uint8_t* data;
    uint32_t size;
    uint32_t entry_point;
} BinProgram;

// Загружает BIN файл из памяти
int bin_load(uint8_t* bin_data, uint32_t size) {
    // Проверка минимального размера
    if (size < 512) {
        terminal_prints("Error: BIN file too small\n");
        return 0;
    }
    
    // Проверка сигнатуры (опционально)
    // Можно добавить свой magic number
    if (bin_data[0] == 0x4D && bin_data[1] == 0x5A) { // 'MZ'
        terminal_prints("Detected MZ executable\n");
    }
    
    terminal_prints("BIN file loaded: ");
    char size_str[16];
    itoa(size, size_str, 10);
    terminal_prints(size_str);
    terminal_prints(" bytes\n");
    
    return 1;
}

// Запускает BIN программу
int bin_execute(uint8_t* bin_data, uint32_t size) {
    if (!bin_load(bin_data, size)) {
        return 0;
    }
    
    terminal_prints("Executing BIN program...\n");
    
    // Сохраняем состояние
    uint32_t old_esp;
    __asm__ volatile ("mov %%esp, %0" : "=r"(old_esp));
    
    // Переключаемся на новый стек для программы (8KB)
    uint8_t program_stack[8192];
    uint32_t new_esp = (uint32_t)(program_stack + 8192);
    
    // Вызываем программу
    // Адрес входа - начало BIN файла (обычно 0x1000)
    void (*program_entry)() = (void(*)())(bin_data);
    
    __asm__ volatile (
        "mov %0, %%esp\n"
        "call *%1\n"
        "mov %2, %%esp\n"
        : : "r"(new_esp), "r"(program_entry), "r"(old_esp)
        : "memory"
    );
    
    terminal_prints("Program exited\n");
    return 1;
}
