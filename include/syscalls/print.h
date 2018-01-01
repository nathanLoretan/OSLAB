
#ifndef __SYSCALLS_PRINT_H__
#define __SYSCALLS_PRINT_H__

#include <common/types.h>

void printf(char* str);
void printfHex(uint8_t key);
void printfHex16(uint16_t key);
void printfHex32(uint32_t key);

#endif  // __SYSCALLS_PRINT_H__
