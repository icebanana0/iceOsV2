#!/bin/bash

# Build the bootloader
nasm -f bin boot/bootloader.asm -o bootloader.bin

gcc -ffreestanding -fno-stack-protector -fno-pie -c kernel/kernel.c -o kernel.o -m32
gcc -ffreestanding -fno-stack-protector -fno-pie -c drivers/ports.c -o ports.o -m32
gcc -ffreestanding -fno-stack-protector -fno-pie -c lib/stdio.c -o stdio.o -m32
gcc -ffreestanding -fno-stack-protector -fno-pie -c lib/string.c -o string.o -m32
gcc -ffreestanding -fno-stack-protector -fno-pie -c drivers/video.c -o video.o -m32
gcc -ffreestanding -fno-stack-protector -fno-pie -c drivers/keyboard.c -o keyboard.o -m32

nasm -f elf32 kernel/kernel.asm -o kernelasm.o

ld -melf_i386 -o kernel.bin -Ttext 0x1000 kernelasm.o kernel.o stdio.o string.o video.o keyboard.o ports.o --oformat binary -T kernel/link.ld

cat bootloader.bin kernel.bin > iceOsV2


qemu-system-x86_64 iceOsV2
