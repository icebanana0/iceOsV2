nasm -f bin boot.asm -o build/boot.bin
gcc -ffreestanding -c kernel.c -o build/kernel.o
ld -o build/kernel.bin -Ttext 0x1000 build/kernel.o --oformat binary
cat build/boot.bin build/kernel.bin > build/iceOs.bin
qemu-system-x86_64 -drive format=raw,file=build/iceOs.bin
