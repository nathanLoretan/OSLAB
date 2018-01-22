/*
*   Peripheral Componenent Interconnect
*/

#ifndef __KERNEL_PCI_H__
#define __KERNEL_PCI_H__

#include <common/types.h>
#include <drivers/io.h>
#include <lib/stdio.h>

struct memoryspace {
    uint8_t  layout          :1;
    uint8_t  type            :3;
    uint8_t  prefetchable    :1;
    uint32_t bar             :28;
} __attribute__ ((packed));
typedef struct memoryspace memoryspace_t;

struct iospace {
    uint8_t  layout          :1;
    uint8_t  reserved        :1;
    uint32_t bar             :30;
} __attribute__ ((packed));
typedef struct iospace iospace_t;

// Base Address Register
typedef union {
    iospace_t iospace;
    memoryspace_t memoryspace;
} bar_t;

struct pci_bar {
    uint8_t  layout          :1;
    uint8_t  type            :3;
    uint8_t  prefetchable    :1;
    uint32_t bar             :27;
} __attribute__ ((packed));
typedef struct pci_bar pci_bar_t;

struct pci_config_data {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
    uint8_t  prog_if;
    uint8_t  revision;
    uint8_t  headertype;
    uint32_t base_addr_0;
    uint32_t base_addr_1;
    uint32_t base_addr_2;
    uint32_t base_addr_3;
    uint32_t base_addr_4;
    uint32_t base_addr_5;
    uint16_t interrupt;
} __attribute__ ((packed));
typedef struct pci_config_data pci_config_data_t;

struct pci_config_addr {
    uint8_t offset      :8;     // Register number, The 2 first bits =0
    uint8_t function    :3;     // Function number
    uint8_t device      :5;     // Device number
    uint8_t bus         :8;     // Bus number
    uint8_t reserved    :7;     // Reserved
    uint8_t enable      :1;     // Enable Bit
} __attribute__ ((packed));
typedef struct pci_config_addr pci_config_addr_t;

void pci_init();
uint32_t pci_read(pci_config_addr_t* config_addr);
void pci_write(pci_config_addr_t* config_addr, uint32_t data);

#endif  // __KERNEL_PCI_H__
