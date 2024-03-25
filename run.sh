nasm -f elf32 boot.asm -o build/boot.o
gcc -m32 -ffreestanding -c kernel.c -o build/kernel.o
ld -m elf_i386 -T linker.ld build/boot.o build/kernel.o -o build/iceOs.elf
qemu-system-i386 -kernel build/iceOs.elf
