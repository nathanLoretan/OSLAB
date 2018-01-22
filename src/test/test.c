#include <test/test.h>


void allocA()
{
    char* text = (char*) malloc(6);
    text[0] = 'H';
    text[1] = 'E';
    text[2] = 'L';
    text[3] = 'L';
    text[4] = 'O';
    text[5] = '\0';
    printf(text);

    while(1) {}
}

void allocB()
{
    char* text = (char*) malloc(6);
    text[0] = 'W';
    text[1] = 'O';
    text[2] = 'R';
    text[3] = 'L';
    text[4] = 'D';
    text[5] = '\0';
    printf(text);

    while(1) {}
}

void allocC()
{
    char* text = (char*) malloc(6);
    text[0] = '!';
    text[1] = '!';
    text[2] = '!';
    text[3] = '!';
    text[4] = '\n';
    text[5] = '\0';
    printf(text);

    while(1) {}
}

void allocD()
{
    char* text = (char*) malloc(8192);

    printf("%x\n", (uint32_t)text); // Print virtual addr of text

    free(text);
    text = NULL;
    text = (char*) malloc(6);

    printf("%x\n", (uint32_t)text); // Print virtual addr of text

    char* text2 = (char*) malloc(6);

    printf("%x\n", (uint32_t)text2); // Print virtual addr of text2

    free(text);
    text = NULL;
    text = (char*) malloc(6);

    printf("%x\n", (uint32_t)text); // Print virtual addr of text

    while(1) {}
}

task_t task_allocA;
task_t task_allocB;
task_t task_allocC;
task_t task_allocD;

void testAlloc()
{
    task_init(&task_allocA, allocA);
    task_init(&task_allocB, allocB);
    task_init(&task_allocC, allocC);
    task_init(&task_allocD, allocD);

    scheduler_add(&task_allocA);
    scheduler_add(&task_allocB);
    scheduler_add(&task_allocC);
    scheduler_add(&task_allocD);
}

void schedulerA()
{
    while(1) {printf("A");}
}

void schedulerB()
{
    while(1) {printf("B");}
}

void schedulerC()
{
    while(1) {printf("C");}
}

task_t task_schedulerA;
task_t task_schedulerB;
task_t task_schedulerC;

void testScheduler()
{
    task_init(&task_schedulerA, schedulerA);
    task_init(&task_schedulerB, schedulerB);
    task_init(&task_schedulerC, schedulerC);

    scheduler_add(&task_schedulerA);
    scheduler_add(&task_schedulerB);
    scheduler_add(&task_schedulerC);
}

void pagingA()
{
    printf("\n");

    int test;
    uint32_t* ptr = &test;
    *ptr = 0xABCDABCD;

    printf("%x\n", (uint32_t)ptr);
    printf("%x\n", (uint32_t)&test);
    printf("%x\n", *ptr);

    // Virtual memory-----------------------------------------------------------
    paging_enable();

    uint32_t* ptr2 = paging_getVirtualAddr(1, 1023, 0xFE0);
    *ptr2 = 0xFFFFFFFF;

    printf("%x\n", (uint32_t)ptr2);
    printf("%x\n", (uint32_t)&test);

    // Virtual memory-----------------------------------------------------------
    paging_disable();

    printf("%x\n", *ptr);

    while(1){}
}

void pagingB()
{
    printf("\n");

    uint32_t* ptr = paging_getPhysicalAddr(0);
    *ptr = 0xABCDABCD;

    printf("%x\n", (uint32_t)ptr);
    printf("%x\n", *ptr);

    // Virtual memory-----------------------------------------------------------
    paging_enable();

    uint32_t* ptr2 = 0x00000000;
    *ptr2 = 0xFFFFFFFF;
    printf("%x\n", (uint32_t)ptr2);

    // Virtual memory-----------------------------------------------------------
    paging_disable();

    printf("%x\n", *ptr);

    while(1){}
}

task_t task_pagingA;
task_t task_pagingB;

void testPaging()
{
    task_init(&task_pagingA, pagingA);
    task_init(&task_pagingB, pagingB);

    scheduler_add(&task_pagingA);
    scheduler_add(&task_pagingB);
}
