#ifndef __TEST_H__
#define __TEST_H__

#include <common/types.h>
#include <kernel/gdt.h>
#include <drivers/pci.h>
#include <kernel/memory/paging.h>
#include <kernel/process/task.h>
#include <kernel/process/scheduler.h>
#include <kernel/interrupt/interrupt_manager.h>
#include <kernel/memory/memory_manager.h>
#include <drivers/pit.h>
#include <drivers/ata.h>
#include <drivers/sata.h>
#include <drivers/keyboard.h>
#include <lib/stdio.h>

void testSata();
void testAta();
void testScheduler();
void testPaging();
void testAlloc();

#endif // __TEST_H__
