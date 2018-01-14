#ifndef __LIB_STDARG_H__
#define __LIB_STDARG_H__

#include <common/types.h>

typedef uint8_t* va_list;

#define va_start(list, param)   list = (((va_list)&param) + sizeof(param))
#define va_arg(list, type)      *(type*)((list += sizeof(uint32_t)) - sizeof(uint32_t))
#define va_end(list)            list = NULL

#endif // __LIB_STDARG_H__
