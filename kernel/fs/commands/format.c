#include <fs.h>
#include <terminal.h>
#include <string.h>

void fs_format(void) {
    terminal_prints("Formatting filesystem...\n");
    fs_count = 1;
    strcpy(fs_cache[0].name, "/");
    fs_cache[0].is_dir = 1;
    fs_cache[0].content[0] = '\0';
    fs_cache[0].size = 0;
    strcpy(current_dir, "/");
    fs_dirty = 1;
    fs_save_to_disk();
    terminal_prints("Filesystem formatted\n");
}
