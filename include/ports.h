#ifndef PORTS
#define PORTS

unsigned char port_byte_read(unsigned short port);
void port_byte_write(unsigned short port, unsigned char data);

#endif