# WexOS Makefile
# Использование: make [target]

.PHONY: all clean run run-hda iso help

# Компилятор и флаги
CC = gcc
LD = ld
CFLAGS = -m32 -ffreestanding -nostdlib -nostartfiles -fno-stack-protector -fno-pie
INCLUDES = -I. -Ikernel -Ikernel/fs -Ikernel/fs/commands -Ikernel/drivers
LDFLAGS = -melf_i386 -T boot/linker.ld

# Директории
KERNEL_DIR = kernel
FS_DIR = kernel/fs
CMD_DIR = kernel/fs/commands
DRIVERS_DIR = kernel/drivers
LIB_DIR = lib
ISO_DIR = iso
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

# Файлы ядра
KERNEL_OBJS = \
    $(KERNEL_DIR)/main.o \
    $(KERNEL_DIR)/terminal.o \
    $(KERNEL_DIR)/keyboard.o \
    $(KERNEL_DIR)/shell.o \
    $(KERNEL_DIR)/reboot.o \
    $(KERNEL_DIR)/bin_loader.o \
    $(DRIVERS_DIR)/io.o \
    $(DRIVERS_DIR)/ata.o \
    $(DRIVERS_DIR)/vga.o \
    $(FS_DIR)/fs.o \
    $(CMD_DIR)/ls.o \
    $(CMD_DIR)/cd.o \
    $(CMD_DIR)/pwd.o \
    $(CMD_DIR)/mkdir.o \
    $(CMD_DIR)/touch.o \
    $(CMD_DIR)/rm.o \
    $(CMD_DIR)/cat.o \
    $(CMD_DIR)/format.o \
    $(LIB_DIR)/string.o

KERNEL_BIN = kernel.bin
ISO_IMAGE = wexos.iso

# Цели
all: $(ISO_IMAGE)

# Сборка ISO
$(ISO_IMAGE): $(KERNEL_BIN)
	@echo "=========================================="
	@echo "Creating ISO image..."
	@mkdir -p $(GRUB_DIR)
	@cp $(KERNEL_BIN) $(BOOT_DIR)/
	@echo "set timeout=2" > $(GRUB_DIR)/grub.cfg
	@echo "set default=0" >> $(GRUB_DIR)/grub.cfg
	@echo 'menuentry "WexOS" { multiboot /boot/kernel.bin boot }' >> $(GRUB_DIR)/grub.cfg
	@grub-mkrescue -o $(ISO_IMAGE) $(ISO_DIR) 2>/dev/null
	@echo "✓ $(ISO_IMAGE) created successfully!"
	@echo ""
	@echo "Run: make run or make run-hda"

# Линковка ядра
$(KERNEL_BIN): $(KERNEL_OBJS)
	@echo "=========================================="
	@echo "Linking kernel..."
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(KERNEL_OBJS)
	@echo "✓ Kernel linked successfully!"

# Компиляция C файлов
$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(DRIVERS_DIR)/%.o: $(DRIVERS_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(FS_DIR)/%.o: $(FS_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(CMD_DIR)/%.o: $(CMD_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIB_DIR)/%.o: $(LIB_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Очистка
clean:
	@echo "Cleaning..."
	@rm -f $(KERNEL_OBJS) $(KERNEL_BIN) $(ISO_IMAGE)
	@echo "✓ Clean completed!"

clean-all: clean
	@rm -rf $(ISO_DIR)
	@echo "✓ Full clean completed!"

# Запуск в QEMU (без диска)
run: $(ISO_IMAGE)
	@echo "=========================================="
	@echo "Starting WexOS in QEMU..."
	@echo "=========================================="
	qemu-system-i386 -cdrom $(ISO_IMAGE) -m 256

# Запуск с диском (с сохранением данных)
run-hda: $(ISO_IMAGE)
	@echo "=========================================="
	@echo "Starting WexOS with disk image..."
	@echo "=========================================="
	@if [ ! -f hda.img ]; then \
		echo "Creating disk image hda.img (32MB)..."; \
		dd if=/dev/zero of=hda.img bs=1M count=32 2>/dev/null; \
	fi
	qemu-system-i386 -cdrom $(ISO_IMAGE) -hda hda.img -m 256 -serial stdio

# Запуск с отладкой
debug: $(ISO_IMAGE)
	@echo "=========================================="
	@echo "Starting WexOS in debug mode..."
	@echo "=========================================="
	qemu-system-i386 -cdrom $(ISO_IMAGE) -s -S -m 256

# Создание диска
create-disk:
	@echo "Creating disk image hda.img (32MB)..."
	dd if=/dev/zero of=hda.img bs=1M count=32
	@echo "✓ Disk created!"

# Справка
help:
	@echo ""
	@echo "WexOS Makefile Commands:"
	@echo "  make          - Build ISO image"
	@echo "  make clean    - Remove object files and binaries"
	@echo "  make clean-all- Remove everything including ISO"
	@echo "  make run      - Run in QEMU (no disk)"
	@echo "  make run-hda  - Run with disk (data persists)"
	@echo "  make debug    - Run with GDB debugging"
	@echo "  make create-disk - Create 32MB disk image"
	@echo "  make help     - Show this help"
	@echo ""
