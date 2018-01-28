#ifndef __LIB_STDIO_H__
#define __LIB_STDIO_H__

#include <common/types.h>
#include <lib/stdarg.h>
#include <lib/stdlib.h>
#include <lib/string.h>

// Possible to calcule the number of argument of __VA_ARGS__, max N-1
#define GET_NBR_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, \
                    _14, _15, _16, _17, _18, _19, _20, N, ...) N

#define COUNT_VARARGS(...) GET_NBR_ARG("ignored", ##__VA_ARGS__, 19, 18, 17, \
                                        16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6,\
                                        5, 4, 3, 2, 1, 0)

// The define allow to get the exact number of argument given to the Function
// printf. The function can't have more than 20 argument
#define printf(str, ...)    print(str, COUNT_VARARGS(__VA_ARGS__) , ##__VA_ARGS__)
void print(const char* str, size_t nbr_args, ...);

#endif // __LIB_STDIO_H__
