#include <fs.h>
#include <terminal.h>
#include <string.h>

void fs_touch(const char* name) {
    if (fs_count >= MAX_FILES) {
        terminal_prints("Error: Max files reached\n");
        return;
    }
    
    char full_path[MAX_PATH];
    if (strcmp(current_dir, "/") == 0) {
        strcpy(full_path, name);
    } else {
        strcpy(full_path, current_dir);
        if (full_path[strlen(full_path)-1] != '/') strcat(full_path, "/");
        strcat(full_path, name);
    }
    
    for (int i = 0; i < fs_count; i++) {
        if (strcmp(fs_cache[i].name, full_path) == 0) {
            terminal_prints("Error: Already exists\n");
            return;
        }
    }
    
    strcpy(fs_cache[fs_count].name, full_path);
    fs_cache[fs_count].is_dir = 0;
    fs_cache[fs_count].content[0] = '\0';
    fs_cache[fs_count].size = 0;
    fs_count++;
    fs_dirty = 1;
    fs_save_to_disk();
    
    terminal_prints("File created: ");
    terminal_prints(name);
    terminal_newline();
}
