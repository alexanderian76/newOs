all:
	nasm -f elf64 boot.asm
	nasm -f elf64 kernel.asm
	nasm -f elf64 long_mode_init.asm
	gcc -ffreestanding -fshort-wchar -m64 -gdwarf-4 -ggdb3 -fno-stack-protector -mno-red-zone -mgeneral-regs-only -c *.cpp
#	gcc -ffreestanding -fshort-wchar -m64 -gdwarf-4 -ggdb3 -fno-stack-protector -mno-red-zone -mgeneral-regs-only -c interrupts/interrupts.cpp -o interrupts.o
	
	
	ld -n -o isofiles/boot/kernel.bin -nmagic -T linker.ld kernel.o boot.o long_mode_init.o main.o 

clean:
	rm kernel.o boot.o main.o utils.o long_mode_init.o 

	rm isofiles/boot/kernel.bin

	
build:
	grub-mkrescue -o os.iso isofiles
	qemu-system-x86_64 -s -cdrom os.iso
