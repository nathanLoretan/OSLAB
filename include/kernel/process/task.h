/*
*   Task
*/

#ifndef __KERNEL_TASK_H__
#define __KERNEL_TASK_H__

#include <common/types.h>
#include <kernel/gdt.h>
#include <kernel/memory/paging.h>
#include <kernel/memory/memory_manager.h>

typedef void (*run_t)();

struct eflags {
    uint8_t cf      :1;     // carry flag
    uint8_t b1      :1;     // nothing
    uint8_t pf      :1;     // parity flag
    uint8_t b3      :1;     // nothing
    uint8_t af      :1;     // auxiliary flag
    uint8_t b5      :1;     // nothing
    uint8_t zf      :1;     // zero flag
    uint8_t sf      :1;     // sign flag

    uint8_t tf      :1;     // trap flag
    uint8_t intf    :1;     // interrupt flag
    uint8_t df      :1;     // direction flag
    uint8_t of      :1;     // overflow flag
    uint8_t iopl    :2;     // i/o priviledge level
    uint8_t nt      :1;     // nested task flag
    uint8_t b15     :1;     // nothing

    uint8_t rf      :1;     // resume flag
    uint8_t vm      :1;     // virtual 8086 mode flag
    uint8_t ac      :1;     // alignment check flag
    uint8_t vif     :1;     // virtual interrupt flag
    uint8_t vip     :1;     // virtual interrupt pending
    uint8_t id      :1;     // id flag
    uint8_t b22_23  :2;     // nothing

    uint8_t b24_31;         // nothing
} __attribute__((packed));
typedef struct eflags eflags_t;

// struct stack_layout { // Interrupt stack layout
//
//     /* Higher address */
//
//     // Saved by the OS
//     // uint32_t gs;    // Custom g segment
//     // uint32_t fs;    // Custom f segment
//     // uint32_t es;    // Extra segment
//     // uint32_t ds;    // Data segment
//
//     uint32_t eax;   // Accumulator
//     uint32_t ebx;   // Base
//     uint32_t ecx;   // Counter
//     uint32_t edx;   // data
//
//     uint32_t esi;   // Source index
//     uint32_t edi;   // Destination index
//     uint32_t ebp;   // Base pointer
//
//     // Saved by the processor
//     uint32_t error;     // Extra information
//     uint32_t eip;       // Instruction pointer
//     uint32_t cs;        // Code segment
//     uint32_t eflags;    // EFLAGS Register
//     uint32_t esp;       // Stack pointer
//     uint32_t ss;        // Stack segment
//
//     /* Lower address */
//
// } __attribute__((packed));
// typedef struct stack_layout stack_layout_t;

struct context {

    /* Higher address */

    // Saved by the OS
    // uint32_t gs;    // Custom g segment
    // uint32_t fs;    // Custom f segment
    // uint32_t es;    // Extra segment
    // uint32_t ds;    // Data segment

    uint32_t cr3;   // Page Directory address

    uint32_t eax;   // Accumulator
    uint32_t ebx;   // Base
    uint32_t ecx;   // Counter
    uint32_t edx;   // data

    uint32_t esi;   // Source index
    uint32_t edi;   // Destination index
    uint32_t ebp;   // Base pointer

    // Saved by the processor
    uint32_t error;     // Extra information
    uint32_t eip;       // Instruction pointer
    uint32_t cs;        // Code segment
    uint32_t eflags;    // EFLAGS Register
    uint32_t esp;       // Stack pointer
    uint32_t ss;        // Stack segment

    /* Lower address */

} __attribute__((packed));
typedef struct context context_t;

struct task {
    run_t      run;
    uint8_t*   heap;
    uint8_t*   stack;
    context_t* context; 
    memory_manager_t memory_manager;
    pt_t* pt;
    pd_t* pd;
} __attribute__((packed));
typedef struct task task_t;

void task_init(task_t* task, run_t run, size_t size);

#endif // __KERNEL_TASK_H__
