nasm -f bin bootloader/boot.asm -o build/boot.bin
gcc -ffreestanding -c kernel/kernel.c -o build/kernel.o
gcc -ffreestanding -c drivers/ports.c -o build/ports.o
# gcc -ffreestanding -c drivers/screen.c -o build/screen.o
ld -o build/kernel.bin -Ttext 0x1000 build/kernel.o build/ports.o --oformat binary
cat build/boot.bin build/kernel.bin > build/iceOs.bin
qemu-system-x86_64 -drive format=raw,file=build/iceOs.bin
