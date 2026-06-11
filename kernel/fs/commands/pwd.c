#include <fs.h>
#include <terminal.h>
#include <string.h>

void pwd_command(void) {
    terminal_prints(current_dir);
    terminal_newline();
}
