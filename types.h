#ifndef TYPES_H
#define TYPES_H

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;

typedef signed long long int64_t;
typedef unsigned long long uint64_t;

typedef char* string;

typedef uint8_t bool;

#define true 1
#define false 0

#define NULL ((void *)0)

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

#if defined(__x86_64__) || defined(_M_X64)
typedef uint64_t size_t;  // Use uint64_t on 64-bit systems
#else
typedef uint32_t size_t;  // Use uint32_t on 32-bit systems
#endif

#endif
