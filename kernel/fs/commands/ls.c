#include <fs.h>
#include <terminal.h>
#include <string.h>
#include <stddef.h>

void fs_ls(void) {
    terminal_prints("\n");
    terminal_prints("Contents of ");
    terminal_prints(current_dir);
    terminal_prints(":\n");
    
    int found = 0;
    
    if (strcmp(current_dir, "/") == 0) {
        for (int i = 0; i < fs_count; i++) {
            if (strcmp(fs_cache[i].name, "/") == 0) continue;
            if (strchr(fs_cache[i].name, '/') == NULL) {
                found++;
                if (fs_cache[i].is_dir) {
                    terminal_setcolor(0x0B);
                    terminal_prints("  [DIR]  ");
                } else {
                    terminal_setcolor(0x0A);
                    terminal_prints("  [FILE] ");
                }
                terminal_prints(fs_cache[i].name);
                if (fs_cache[i].is_dir) terminal_prints("/");
                terminal_setcolor(0x07);
                terminal_newline();
            }
        }
    } else {
        int current_len = strlen(current_dir);
        for (int i = 0; i < fs_count; i++) {
            if (strncmp(fs_cache[i].name, current_dir, current_len) == 0) {
                char* relative = fs_cache[i].name + current_len;
                if (*relative == '/') relative++;
                if (*relative != '\0' && strchr(relative, '/') == NULL) {
                    found++;
                    if (fs_cache[i].is_dir) {
                        terminal_setcolor(0x0B);
                        terminal_prints("  [DIR]  ");
                    } else {
                        terminal_setcolor(0x0A);
                        terminal_prints("  [FILE] ");
                    }
                    terminal_prints(relative);
                    if (fs_cache[i].is_dir) terminal_prints("/");
                    terminal_setcolor(0x07);
                    terminal_newline();
                }
            }
        }
    }
    
    if (found == 0) {
        terminal_setcolor(0x0E);
        terminal_prints("  (empty)");
        terminal_setcolor(0x07);
        terminal_newline();
    }
    terminal_newline();
}
