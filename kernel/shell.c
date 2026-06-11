#include <stddef.h>

#include "shell.h"
#include "terminal.h"
#include "keyboard.h"
#include "fs.h"
#include "reboot.h"
#include "string.h"

#define MAX_HISTORY 10
#define MAX_CMD_LEN 128

static char history[MAX_HISTORY][MAX_CMD_LEN];
static int history_count = 0;
static int history_pos = -1;

void explorer_command(void);

void show_prompt(void) {
    terminal_prints("[");
    terminal_prints(current_dir);
    terminal_prints("] $ ");
}

void help_command(void) {
    terminal_prints("\n");
    terminal_prints("Available commands:\n");
    terminal_prints("  help      - Show this help\n");
    terminal_prints("  clear     - Clear screen\n");
    terminal_prints("  reboot    - Reboot system\n");
    terminal_prints("  shutdown  - Shutdown\n");
    terminal_prints("  ls        - List files\n");
    terminal_prints("  cd <dir>  - Change directory\n");
    terminal_prints("  mkdir <d> - Create directory\n");
    terminal_prints("  touch <f> - Create file\n");
    terminal_prints("  rm <name> - Remove file/dir\n");
    terminal_prints("  cat <f>   - Show file content\n");
    terminal_prints("  pwd       - Show current directory\n");
    terminal_prints("  format    - Format filesystem\n");
    terminal_prints("\n");
}

void run_command(char* cmd) {
    char* end = cmd + strlen(cmd) - 1;
    while (end > cmd && *end == ' ') end--;
    *(end + 1) = '\0';
    
    if (strlen(cmd) == 0) return;
    
    if (history_count < MAX_HISTORY) {
        strcpy(history[history_count], cmd);
        history_count++;
    }
    
    char* space = cmd;
    while (*space && *space != ' ') space++;
    
    if (*space) {
        *space = '\0';
        space++;
        while (*space == ' ') space++;
    }
    
    if (strcmp(cmd, "help") == 0) help_command();
    else if (strcmp(cmd, "clear") == 0) terminal_clear();
    else if (strcmp(cmd, "reboot") == 0) reboot_system();
    else if (strcmp(cmd, "shutdown") == 0) shutdown_system();
    else if (strcmp(cmd, "ls") == 0) fs_ls();
    else if (strcmp(cmd, "cd") == 0) { if (space && *space) fs_cd(space); else terminal_prints("Usage: cd <dir>\n"); }
    else if (strcmp(cmd, "pwd") == 0) pwd_command();
    else if (strcmp(cmd, "mkdir") == 0) { if (space && *space) fs_mkdir(space); else terminal_prints("Usage: mkdir <name>\n"); }
    else if (strcmp(cmd, "touch") == 0) { if (space && *space) fs_touch(space); else terminal_prints("Usage: touch <name>\n"); }
    else if (strcmp(cmd, "rm") == 0) { if (space && *space) fs_rm(space); else terminal_prints("Usage: rm <name>\n"); }
    else if (strcmp(cmd, "cat") == 0) { if (space && *space) fs_cat(space); else terminal_prints("Usage: cat <file>\n"); }
    else if (strcmp(cmd, "format") == 0) fs_format();
    else {
        terminal_prints("Unknown command: ");
        terminal_prints(cmd);
        terminal_prints("\n");
    }
}

void shell_loop(void) {
    char cmd_buf[MAX_CMD_LEN];
    int cmd_idx = 0;
    
    while (1) {
        show_prompt();
        cmd_idx = 0;
        history_pos = -1;
        
        while (1) {
            char c = keyboard_getch_with_arrows();
            
            if (c == '\n') {
                cmd_buf[cmd_idx] = '\0';
                terminal_newline();
                if (cmd_idx > 0) run_command(cmd_buf);
                break;
            }
            else if (c == '\b') {
                if (cmd_idx > 0) {
                    cmd_idx--;
                    terminal_putchar('\b');
                    terminal_putchar(' ');
                    terminal_putchar('\b');
                }
            }
            else if (c == 'U') {
                if (history_pos < history_count - 1) {
                    history_pos++;
                    for (int i = 0; i < cmd_idx; i++) terminal_putchar('\b');
                    for (int i = 0; i < cmd_idx; i++) terminal_putchar(' ');
                    for (int i = 0; i < cmd_idx; i++) terminal_putchar('\b');
                    strcpy(cmd_buf, history[history_count - 1 - history_pos]);
                    cmd_idx = strlen(cmd_buf);
                    terminal_prints(cmd_buf);
                }
            }
            else if (c == 'D') {
                if (history_pos > 0) {
                    history_pos--;
                    for (int i = 0; i < cmd_idx; i++) terminal_putchar('\b');
                    for (int i = 0; i < cmd_idx; i++) terminal_putchar(' ');
                    for (int i = 0; i < cmd_idx; i++) terminal_putchar('\b');
                    strcpy(cmd_buf, history[history_count - 1 - history_pos]);
                    cmd_idx = strlen(cmd_buf);
                    terminal_prints(cmd_buf);
                } else if (history_pos == 0) {
                    history_pos = -1;
                    for (int i = 0; i < cmd_idx; i++) terminal_putchar('\b');
                    for (int i = 0; i < cmd_idx; i++) terminal_putchar(' ');
                    for (int i = 0; i < cmd_idx; i++) terminal_putchar('\b');
                    cmd_idx = 0;
                    cmd_buf[0] = '\0';
                }
            }
            else if (c >= 32 && c <= 126 && cmd_idx < MAX_CMD_LEN - 1) {
                cmd_buf[cmd_idx] = c;
                cmd_idx++;
                terminal_putchar(c);
            }
        }
    }
}

// Обработка ls с параметрами
void ls_command(char* arg) {
    if (arg && strcmp(arg, "-l") == 0) {
        // Подробный вывод
        terminal_prints("\n");
        terminal_prints("Permission  Size  Name\n");
        terminal_prints("----------  ----  ----\n");
        
        for (int i = 0; i < fs_count; i++) {
            if (strcmp(fs_cache[i].name, "/") == 0) continue;
            
            if (strstr(fs_cache[i].name, current_dir) == fs_cache[i].name) {
                char* name = fs_cache[i].name + strlen(current_dir);
                if (*name == '/') name++;
                if (*name != '\0' && strchr(name, '/') == NULL) {
                    // Права доступа
                    if (fs_cache[i].is_dir) terminal_prints("drwxr-xr-x");
                    else terminal_prints("-rw-r--r--");
                    terminal_prints("  ");
                    
                    // Размер
                    char size_str[8];
                    itoa(fs_cache[i].size, size_str, 10);
                    terminal_prints(size_str);
                    for (int j = strlen(size_str); j < 6; j++) terminal_putchar(' ');
                    
                    // Имя
                    if (fs_cache[i].is_dir) terminal_setcolor(0x0B);
                    else terminal_setcolor(0x0F);
                    terminal_prints(name);
                    if (fs_cache[i].is_dir) terminal_prints("/");
                    terminal_setcolor(0x07);
                    terminal_newline();
                }
            }
        }
        terminal_newline();
    } else {
        fs_ls();
    }
}
