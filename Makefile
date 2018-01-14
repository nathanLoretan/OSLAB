GCCPARAMS = -m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = 	obj/boot/bootloader.o \
			obj/kernel/kernel.o \
			obj/drivers/pci.o \
			obj/kernel/gdt.o \
			obj/kernel/process/task.o \
			obj/kernel/process/scheduler.o \
			obj/kernel/memory/paging.o \
			obj/kernel/memory/memory_manager.o \
			obj/kernel/interrupt/pic.o \
			obj/kernel/interrupt/idt.o \
			obj/kernel/interrupt/interrupts.o \
			obj/kernel/interrupt/interrupt_manager.o \
			obj/drivers/io.o \
			obj/drivers/ata.o \
			obj/drivers/pit.o \
			obj/drivers/keyboard.o \
			obj/common/queue.o \
			obj/common/event.o \
			obj/common/stack.o \
			obj/common/list.o \
			obj/common/timer.o \
			obj/common/hashtable.o \
			obj/lib/stdarg.o \
			obj/lib/stdio.o \
			obj/lib/stdlib.o \
			obj/lib/string.o \
			obj/test/test.o

run: oslab.iso
	@(killall VirtualBox && sleep 1) || true
	@VirtualBox --startvm 'OSLAB' &

obj/%.o: src/%.c
	@mkdir -p $(@D)
	@gcc $(GCCPARAMS) -c -o $@ $<

obj/%.o: src/%.asm
	@mkdir -p $(@D)
	@as $(ASPARAMS) -o $@ $<

oslab.bin: linker.ld $(objects)
	@ld $(LDPARAMS) -T $< -o $@ $(objects)

oslab.iso: oslab.bin
	@mkdir -p iso
	@mkdir -p iso/boot
	@mkdir -p iso/boot/grub
	@cp oslab.bin iso/boot/oslab.bin
	@echo 'set timeout=0'					>  iso/boot/grub/grub.cfg
	@echo 'set default=0'					>> iso/boot/grub/grub.cfg
	@echo ''								>> iso/boot/grub/grub.cfg
	@echo 'menuentry "OSLAB" {'				>> iso/boot/grub/grub.cfg
	@echo '  multiboot /boot/oslab.bin'		>> iso/boot/grub/grub.cfg
	@echo '  boot'							>> iso/boot/grub/grub.cfg
	@echo '}'								>> iso/boot/grub/grub.cfg
	@grub-mkrescue --output=$@ iso
	@rm -rf iso

.PHONY: clean
clean:
	rm -rf obj oslab.bin oslab.iso
