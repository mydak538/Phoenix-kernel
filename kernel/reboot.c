#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

void reboot_system(void) {
    // Способ 1: через клавиатуру
    outb(0x64, 0xFE);
    
    // Способ 2: через тройной fault
    __asm__ volatile(
        "cli\n"
        "mov $0xdeadbeef, %%eax\n"
        "jmp *%%eax\n"  // Исправлено: добавлена звездочка
        : : : "eax"
    );
    
    while(1) { __asm__ volatile("hlt"); }
}

void shutdown_system(void) {
    // Для QEMU
    outw(0x604, 0x2000);
    outw(0x4004, 0x3400);
    
    reboot_system();
}
