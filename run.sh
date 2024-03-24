nasm -fbin src/boot.asm -o build/boot.bin
gcc -ffreestanding -c src/kernel.c -o build/kernel.o
ld -o build/kernel.bin -Ttext 0x1000 build/kernel.o --oformat binary
cat build/boot.bin build/kernel.bin > build/ice.bin
qemu-system-i386 build/ice.bin
