#include "../typeDef.h"

void outb(uint16_t port, uint8_t data){
	asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
	return;
}

uint8_t inb(uint16_t port){
	uint8_t res;
	asm volatile("inb %1, %0" : "=a"(res) : "Nd"(port));
	return res;
}

void outw(uint16_t port, uint16_t value)
{
	asm volatile ("outw %w0, %1" : : "a" (value), "id" (port) );
}

uint16_t inw(uint16_t port){
   uint16_t ret;
   asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
} 
