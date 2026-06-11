#include <fs.h>
#include <terminal.h>
#include <string.h>
#include <stddef.h>

void fs_cd(const char* name) {
    if (strcmp(name, "..") == 0) {
        if (strcmp(current_dir, "/") != 0) {
            char* last_slash = strrchr(current_dir, '/');
            if (last_slash != NULL) {
                if (last_slash == current_dir) {
                    strcpy(current_dir, "/");
                } else {
                    *last_slash = '\0';
                    if (strlen(current_dir) == 0) strcpy(current_dir, "/");
                }
            }
        }
    } else if (strcmp(name, "/") == 0) {
        strcpy(current_dir, "/");
    } else {
        char full_path[MAX_PATH];
        if (strcmp(current_dir, "/") == 0) {
            strcpy(full_path, name);
        } else {
            strcpy(full_path, current_dir);
            if (full_path[strlen(full_path)-1] != '/') strcat(full_path, "/");
            strcat(full_path, name);
        }
        
        for (int i = 0; i < fs_count; i++) {
            if (strcmp(fs_cache[i].name, full_path) == 0 && fs_cache[i].is_dir) {
                strcpy(current_dir, full_path);
                if (current_dir[strlen(current_dir)-1] != '/') strcat(current_dir, "/");
                return;
            }
        }
        terminal_prints("Error: Directory not found\n");
    }
}
