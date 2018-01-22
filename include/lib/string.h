#ifndef __LIB_STRING_H__
#define __LIB_STRING_H__

#include <common/types.h>

void* memset (void* ptr, int value, size_t num);

void* memcpy (void* dest, const void* src, size_t num);
char* strcpy( char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t num);

int strcmp(const char* str1, const char* str2);
int memcmp(const void* ptr1, const void* ptr2, size_t num);

#endif // __LIB_STRING_H__
