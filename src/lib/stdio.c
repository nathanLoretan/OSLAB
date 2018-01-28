#include <lib/stdio.h>
#include <kernel/memory/memory_layout.h>

#define SCREEN_WIDTH   80
#define SCREEN_HEIGHT  25

static uint16_t* videoMemory = (uint16_t*)VIDEO_MEMORY;
static uint8_t x = 0;   // Position x on the screen
static uint8_t y = 0;   // Position y on the screen

void pChar(const char key)
{
    switch(key)
    {
        case '\n': {
                x = 0;
                y++;
            } break;

            default: {
                videoMemory[80 * y + x] = (videoMemory[ 80 * y + x] & 0xFF00) | key;
                x++;
            } break;
    }

    if(x >= 80)
    {
        x=0;
        y++;
    }

    if(y >= 25)
    {
        for(y = 0; y < 25; y++)
            for(x = 0; x < 80; x++)
                videoMemory[80*y+x] = (videoMemory[80*y+x] & 0xFF00) | ' ';
        x = 0;
        y = 0;
    }
}

void pText(const char* str)
{
    for(int i = 0; str[i] != '\0'; i++) {
        pChar(str[i]);
    }
}

static void pHex(const uint8_t key)
{
    char* hex = "0123456789ABCDEF";

    pChar(hex[(key >> 4) & 0xF]);
    pChar(hex[ key       & 0xF]);
}

static void pHex8(const uint8_t key)
{
    char* hex = "0123456789ABCDEF";

    pChar('0');
    pChar('x');
    pHex(key);
}

static void pHex16(const uint16_t key)
{
    pChar('0');
    pChar('x');
    pHex((key >> 8) & 0xFF);
    pHex( key       & 0xFF);
}
static void pHex32(const uint32_t key)
{
    pChar('0');
    pChar('x');
    pHex((key >> 24) & 0xFF);
    pHex((key >> 16) & 0xFF);
    pHex((key >> 8)  & 0xFF);
    pHex( key        & 0xFF);
}

void print(const char* str, size_t nbr_args, ...)
{
    va_list vl;
    uint32_t arg_idx = 0;   // argument index

    va_start(vl, nbr_args);

    // Parse the string to find the different element
    for(size_t i = 0;  str[i] != '\0'; i++)
    {
        if(str[i] == '%' && str[i+1] == 'p' && arg_idx < nbr_args) {
            pHex32(va_arg(vl, uint32_t)); // Print the argument
            i+=2;                         // Correct the index of the string
            arg_idx++;
        }
        if(str[i] == '%' && str[i+1] == 's' && arg_idx < nbr_args) {
            pText(va_arg(vl, char*));     // Print the argument
            i+=2;                         // Correct the index of the string
            arg_idx++;
        }
        if(str[i] == '%' && str[i+1] == 'x' && arg_idx < nbr_args) {
            pHex32(va_arg(vl, uint32_t)); // Print the argument
            i+=2;                         // Correct the index of the string
            arg_idx++;
        }
        if(str[i] == '%' && str[i+1] == '8' && str[i+2] == 'x' && arg_idx < nbr_args) {
            pHex8(va_arg(vl, uint8_t)); // Print the argument
            i+=3;                        // Correct the index of the string
            arg_idx++;
        }
        if(str[i] == '%' && str[i+1] == '1' && str[i+2] == '6' && str[i+3] == 'x' && arg_idx < nbr_args) {
            pHex16(va_arg(vl, uint16_t)); // Print the argument
            i+=4;                         // Correct the index of the string
            arg_idx++;
        }
        if(str[i] == '%' && str[i+1] == '3' && str[i+2] == '2' && str[i+3] == 'x' && arg_idx < nbr_args) {
            pHex32(va_arg(vl, uint32_t)); // Print the argument
            i+=3;                         // Correct the index of the string
            arg_idx++;
        }
        else {
            pChar(str[i]);
        }

        if(str[i] == '\0') {
            break;
        }
    }

    va_end(vl);
}
