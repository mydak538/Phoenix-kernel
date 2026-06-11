#include <fs.h>
#include <terminal.h>
#include <string.h>

void fs_rm(const char* name) {
    char full_path[MAX_PATH];
    if (strcmp(current_dir, "/") == 0) {
        strcpy(full_path, name);
    } else {
        strcpy(full_path, current_dir);
        if (full_path[strlen(full_path)-1] != '/') strcat(full_path, "/");
        strcat(full_path, name);
    }
    
    int found = -1;
    for (int i = 0; i < fs_count; i++) {
        if (strcmp(fs_cache[i].name, full_path) == 0) {
            found = i;
            break;
        }
    }
    
    if (found == -1) {
        terminal_prints("Error: Not found\n");
        return;
    }
    
    for (int i = found; i < fs_count - 1; i++) {
        fs_cache[i] = fs_cache[i + 1];
    }
    fs_count--;
    fs_dirty = 1;
    fs_save_to_disk();
    
    terminal_prints("Removed: ");
    terminal_prints(name);
    terminal_newline();
}
