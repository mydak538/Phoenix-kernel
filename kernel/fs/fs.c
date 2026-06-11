#include <fs.h>
#include <terminal.h>
#include <string.h>
#include <stddef.h>
#include <drivers/ata.h>

#define SECTOR_SIZE 512
#define FS_START_SECTOR 100
#define FS_MAGIC 0x57465853

FSNode fs_cache[MAX_FILES];
int fs_count = 0;
char current_dir[MAX_PATH] = "/";
int fs_dirty = 0;

void fs_load_from_disk(void) {
    uint8_t buffer[SECTOR_SIZE];
    terminal_prints("[FS] Loading from disk...\n");
    ata_read_sector(FS_START_SECTOR, buffer);
    
    if (*((uint32_t*)buffer) == FS_MAGIC) {
        fs_count = *((uint32_t*)(buffer + 4));
        if (fs_count > MAX_FILES) fs_count = MAX_FILES;
        
        int node_size = sizeof(FSNode);
        int sectors_per_node = (node_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
        
        for (int i = 0; i < fs_count; i++) {
            int sector = FS_START_SECTOR + 1 + i * sectors_per_node;
            uint8_t* node_ptr = (uint8_t*)&fs_cache[i];
            
            for (int s = 0; s < sectors_per_node; s++) {
                ata_read_sector(sector + s, buffer);
                int offset = s * SECTOR_SIZE;
                int copy_size = (offset + SECTOR_SIZE <= node_size) ? SECTOR_SIZE : node_size - offset;
                if (copy_size > 0) {
                    memcpy(node_ptr + offset, buffer, copy_size);
                }
            }
        }
        terminal_prints("[FS] Loaded ");
        char buf[10];
        itoa(fs_count, buf, 10);
        terminal_prints(buf);
        terminal_prints(" files\n");
    } else {
        terminal_prints("[FS] No filesystem found, creating new...\n");
        fs_count = 1;
        strcpy(fs_cache[0].name, "/");
        fs_cache[0].is_dir = 1;
        fs_cache[0].content[0] = '\0';
        fs_cache[0].size = 0;
        fs_dirty = 1;
        fs_save_to_disk();
    }
}

void fs_save_to_disk(void) {
    if (!fs_dirty) return;
    
    uint8_t buffer[SECTOR_SIZE];
    int node_size = sizeof(FSNode);
    int sectors_per_node = (node_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    
    terminal_prints("[FS] Saving to disk...\n");
    
    memset(buffer, 0, SECTOR_SIZE);
    *((uint32_t*)buffer) = FS_MAGIC;
    *((uint32_t*)(buffer + 4)) = fs_count;
    ata_write_sector(FS_START_SECTOR, buffer);
    
    for (int i = 0; i < fs_count; i++) {
        int sector = FS_START_SECTOR + 1 + i * sectors_per_node;
        uint8_t* node_ptr = (uint8_t*)&fs_cache[i];
        
        for (int s = 0; s < sectors_per_node; s++) {
            memset(buffer, 0, SECTOR_SIZE);
            int offset = s * SECTOR_SIZE;
            int copy_size = (offset + SECTOR_SIZE <= node_size) ? SECTOR_SIZE : node_size - offset;
            if (copy_size > 0) {
                memcpy(buffer, node_ptr + offset, copy_size);
            }
            ata_write_sector(sector + s, buffer);
        }
    }
    
    fs_dirty = 0;
    terminal_prints("[FS] Saved successfully\n");
}

void fs_init(void) {
    terminal_prints("[FS] Initializing...\n");
    ata_init();
    fs_load_from_disk();
    strcpy(current_dir, "/");
    terminal_prints("[FS] Ready\n");
}

FSNode* fs_find_file(const char* name) {
    char full_path[MAX_PATH];
    if (strcmp(current_dir, "/") == 0) {
        strcpy(full_path, name);
    } else {
        strcpy(full_path, current_dir);
        if (full_path[strlen(full_path)-1] != '/') strcat(full_path, "/");
        strcat(full_path, name);
    }
    
    for (int i = 0; i < fs_count; i++) {
        if (strcmp(fs_cache[i].name, full_path) == 0 && !fs_cache[i].is_dir) {
            return &fs_cache[i];
        }
    }
    return NULL;
}

void fs_sync(void) {
    if (fs_dirty) {
        fs_save_to_disk();
    }
}
