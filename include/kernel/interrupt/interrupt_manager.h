/*
*   Interrupt Manager
*/

#ifndef __KERNEL_INTERRUPT_MANAGER_H__
#define __KERNEL_INTERRUPT_MANAGER_H__

#include <common/types.h>
#include <common/timer.h>
#include <drivers/io.h>
#include <kernel/gdt.h>
#include <kernel/process/scheduler.h>
#include <kernel/interrupt/idt.h>
#include <kernel/interrupt/pic.h>
#include <syscalls/print.h>

typedef void (*isr_t)();

void im_init();
void im_enable_interrupt();
void im_disable_interrupt();
void im_subscribe(isr_t handler, uint8_t interrupt);
isr_t im_queue_get();
volatile bool_t im_queue_isEmpty();

#endif // __KERNEL_INTERRUPT_MANAGER_H__
