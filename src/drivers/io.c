/*
*   I/O
*/

#include <drivers/io.h>

inline uint8_t io_read8(uint16_t port_number)
{
    uint8_t data;
    __asm__ volatile("inb %1, %0" : "=a" (data) : "Nd" (port_number));  // in byte
    return data;
}

inline void io_write8(uint16_t port_number, uint8_t data) {
    __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port_number)); // out byte
}

inline void io_write8Slow(uint16_t port_number, uint8_t data) {
    __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (data), "Nd" (port_number));
}

inline uint16_t io_read16(uint16_t port_number)
{
    uint16_t data;
    __asm__ volatile("inw %1, %0" : "=a" (data) : "Nd" (port_number));  // in word
    return data;
}

inline void io_write16(uint16_t port_number, uint16_t data) {
    __asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port_number)); // out byte
}

inline uint32_t io_read32(uint16_t port_number)
{
    uint32_t data;
    __asm__ volatile("inl %1, %0" : "=a" (data) : "Nd" (port_number));  // in long
    return data;
}

inline void io_write32(uint16_t port_number, uint32_t data) {
    __asm__ volatile("outl %0, %1" : : "a"(data), "Nd" (port_number));  // out long
}
