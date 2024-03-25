export PATH=$PATH:/usr/local/i386elfgcc/bin

nasm "bootloader/boot.asm" -f bin -o "build/boot.bin"
nasm "bootloader/kernelEntry.asm" -f elf -o "build/kernelEntry.o"
i386-elf-gcc -ffreestanding -m32 -g -c "kernel/kernel.c" -o "build/kernel.o"
nasm "bootloader/zeroes.asm" -f bin -o "build/zeroes.bin"

i386-elf-ld -o "build/fullKernel.bin" -Ttext 0x1000 "build/kernelEntry.o" "build/kernel.o" --oformat binary

cat "build/boot.bin" "build/fullKernel.bin" "build/zeroes.bin"  > "build/iceOs.bin"

qemu-system-x86_64 -drive format=raw,file="build/iceOs.bin",index=0,if=floppy,  -m 128M