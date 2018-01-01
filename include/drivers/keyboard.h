/*
*   Keyboard
*/

#ifndef __DRIVERS_KEYBOARD_H__
#define __DRIVERS_KEYBOARD_H__

#include <common/types.h>
#include <drivers/io.h>
#include <syscalls/print.h>
#include <kernel/interrupt/interrupt_manager.h>
#include <kernel/interrupt/pic.h>

void kb_init();

#endif  // __DRIVERS_KEYBOARD_H__
