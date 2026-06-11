#ifndef FS_H
#define FS_H

#include <stdint.h>

#define MAX_NAME 256
#define MAX_PATH 1024
#define MAX_FILES 64

typedef struct {
    char name[MAX_PATH];
    int is_dir;
    char content[4096];
    uint32_t size;
} FSNode;

extern FSNode fs_cache[MAX_FILES];
extern int fs_count;
extern char current_dir[MAX_PATH];
extern int fs_dirty;

void fs_init(void);
void fs_save_to_disk(void);
void fs_load_from_disk(void);
FSNode* fs_find_file(const char* name);
void fs_sync(void);

// Команды
void fs_ls(void);
void fs_cd(const char* name);
void pwd_command(void);
void fs_mkdir(const char* name);
void fs_touch(const char* name);
void fs_rm(const char* name);
void fs_cat(const char* filename);
void fs_format(void);

#endif
