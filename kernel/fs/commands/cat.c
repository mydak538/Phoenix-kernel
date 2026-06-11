#include <fs.h>
#include <terminal.h>
#include <string.h>

void fs_cat(const char* filename) {
    FSNode* file = fs_find_file(filename);
    if (!file) {
        terminal_prints("Error: File not found\n");
        return;
    }
    if (file->size > 0) {
        terminal_prints(file->content);
        terminal_newline();
    } else {
        terminal_prints("File is empty\n");
    }
}
