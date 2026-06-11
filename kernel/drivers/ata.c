#include <ata.h>
#include <terminal.h>
#include <io.h>
#include <string.h>

#define ATA_DATA 0x1F0
#define ATA_SECTORS 0x1F2
#define ATA_LBA_LOW 0x1F3
#define ATA_LBA_MID 0x1F4
#define ATA_LBA_HIGH 0x1F5
#define ATA_DRIVE 0x1F6
#define ATA_STATUS 0x1F7
#define ATA_COMMAND 0x1F7

static int ata_present = 0;

static void ata_wait_ready(void) {
    for (int i = 0; i < 100000; i++) {
        if (!(inb(ATA_STATUS) & 0x80)) {
            return;
        }
    }
}

static void ata_wait_drq(void) {
    for (int i = 0; i < 100000; i++) {
        if (inb(ATA_STATUS) & 0x08) {
            return;
        }
    }
}

void ata_init(void) {
    terminal_prints("ATA: Detecting disk...\n");
    
    outb(0x3F6, 0x04);
    for (int i = 0; i < 1000; i++);
    outb(0x3F6, 0x00);
    
    outb(ATA_DRIVE, 0xA0);
    ata_wait_ready();
    
    uint8_t status = inb(ATA_STATUS);
    if (status != 0xFF && status != 0x00) {
        ata_present = 1;
        terminal_prints("ATA: Disk detected\n");
    } else {
        terminal_prints("ATA: No disk found, using memory only\n");
    }
}

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    if (!ata_present) {
        memset(buffer, 0, SECTOR_SIZE);
        return;
    }
    
    ata_wait_ready();
    
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTORS, 1);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, 0x20);
    
    ata_wait_ready();
    ata_wait_drq();
    
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_DATA);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
    }
}

void ata_write_sector(uint32_t lba, uint8_t* buffer) {
    if (!ata_present) return;
    
    ata_wait_ready();
    
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTORS, 1);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, 0x30);
    
    ata_wait_ready();
    ata_wait_drq();
    
    for (int i = 0; i < 256; i++) {
        uint16_t data = (buffer[i * 2 + 1] << 8) | buffer[i * 2];
        outw(ATA_DATA, data);
    }
    
    ata_wait_ready();
}
