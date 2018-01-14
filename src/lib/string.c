#include <lib/string.h>

void* memcpy(void* dest, const void* src, size_t num)
{
    bool_t isOdd  = num % 2;
    size_t middle = num / 2;

    uint8_t* ptr1 = (uint8_t*) dest;
    uint8_t* ptr2 = (uint8_t*) src;

    for(size_t i = 0; i < middle; i++)
    {
        ptr1[i] = ptr2[i];
        ptr1[num - i - 1] = ptr2[num - i - 1];
    }

    if(isOdd)
    {
        ptr1[middle] = ptr2[middle];
    }

    return (void*) ptr1;
}

char* strcpy(char* dest, const char* src)
{
    for(size_t i = 0; dest[i] != '\0' && src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }

    return dest;
}

char* strncpy(char* dest, const char* src, size_t num)
{
    bool_t isOdd  = num % 2;
    size_t middle = num / 2;

    for(size_t i = 0; i < middle; i++)
    {
        dest[i] = src[i];
        dest[num - i - 1] = src[num - i - 1];
    }

    if(isOdd)
    {
        dest[middle] = src[middle];
    }

    return dest;
}

int strcmp(const char* str1, const char* str2)
{
    // The function return:
    // 0  -> The contents of both strings are equal
    // -1 -> The first character that does not match has a lower value in str1
    //       than in str2
    // 1  -> The first character that does not match has a greater value in str1
    //       than in str2

    size_t i = 0;

    while(str1[i] != '\0' && str2[i] != '\0')
    {
        if(str1[i] > str2[i]) {
            return 1;
        }

        if(str1[i] < str2[i]) {
            return -1;
        }

        i++;
    }

    if(str1[i] == '\0' && str2[i] != '\0') {
        return 1;
    }

    if(str1[i] != '\0' && str2[i] == '\0') {
        return -1;
    }

    return 0;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
    // The function return:
    // 0  -> The contents of both strings are equal
    // -1 -> The first character that does not match has a lower value in ptr1
    //       than in ptr2
    // 1  -> The first character that does not match has a greater value in ptr1
    //       than in ptr2

    uint8_t* el1 = (uint8_t*) ptr1;
    uint8_t* el2 = (uint8_t*) ptr2;

    for(size_t i = 0; i < num; i++)
    {
        if(el1[i] > el2[i]) {
            return 1;
        }

        if(el1[i] < el2[i]) {
            return -1;
        }
    }

    return 0;
}
