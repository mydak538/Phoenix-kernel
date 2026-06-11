#ifndef ATA_H
#define ATA_H

#include <stdint.h>

#define SECTOR_SIZE 512

void ata_init(void);
void ata_read_sector(uint32_t lba, uint8_t* buffer);
void ata_write_sector(uint32_t lba, uint8_t* buffer);

#endif
