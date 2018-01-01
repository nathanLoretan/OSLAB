/*
*   Peripheral Componenent Interconnect
*/

#ifndef __KERNEL_PCI_H__
#define __KERNEL_PCI_H__

#include <common/types.h>
#include <drivers/io.h>
#include <syscalls/print.h>

#define PCI_BUS_NBR         8
#define PCI_DEVICE_NBR      32
#define PCI_FUNCTION_NBR    8

#define PCI_OFFSET_VENDOR_ID    0x00
#define PCI_OFFSET_DEVICE_ID    0x02
#define PCI_OFFSET_REVISION     0x08
#define PCI_OFFSET_PROG_IF      0x09
#define PCI_OFFSET_SUBCLASS     0x0A
#define PCI_OFFSET_CLASS        0x0B
#define PCI_OFFSET_HEADERTYPE   0x0E
#define PCI_OFFSET_MF           0x0E    // Multiple function
#define PCI_OFFSET_BAR_0        0x10
#define PCI_OFFSET_BAR_1        0x14
#define PCI_OFFSET_BAR_2        0x18
#define PCI_OFFSET_BAR_3        0x1C
#define PCI_OFFSET_BAR_4        0x20
#define PCI_OFFSET_BAR_5        0x24
#define PCI_OFFSET_INTERRUPT    0x3C

#define PCI_MASK_VENDOR_ID    0x0000FFFF
#define PCI_MASK_DEVICE_ID    0x0000FFFF
#define PCI_MASK_REVISION     0x000000FF
#define PCI_MASK_PROG_IF      0x000000FF
#define PCI_MASK_SUBCLASS     0x000000FF
#define PCI_MASK_CLASS        0x000000FF
#define PCI_MASK_HEADERTYPE   0x0000007F
#define PCI_MASK_MF           0x00000080    // Multiple function
#define PCI_MASK_BAR          0xFFFFFFFF
#define PCI_MASK_INTERRUPT    0x000000FF

#define PCI_BAR_IOSPACE       0x00000001
#define PCI_BAR_MEMORYSPACE   0x00000000

#define PCI_INVALID_VENDOR_ID   0xFFFF

struct memoryspace {
    uint8_t  layout          :1;
    uint8_t  type            :3;
    uint8_t  prefetchable    :1;
    uint32_t bar             :28;
} __attribute__ ((packed));

struct iospace {
    uint8_t  layout          :1;
    uint8_t  reserved        :1;
    uint32_t bar             :30;
} __attribute__ ((packed));

typedef struct iospace     iospace_t;
typedef struct memoryspace memoryspace_t;

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

struct pci_config_addr {
    uint8_t offset      :8;     // Register number, The 2 first bits =0
    uint8_t function    :3;     // Function number
    uint8_t device      :5;     // Device number
    uint8_t bus         :8;     // Bus number
    uint8_t reserved    :7;     // Reserved
    uint8_t enable      :1;     // Enable Bit
} __attribute__ ((packed));

struct pci_device {
    struct pci_config_data config_data;
    struct pci_config_addr config_addr;
} __attribute__ ((packed));

typedef struct pci_device      pci_device_t;
typedef struct pci_config_data pci_config_data_t;
typedef struct pci_config_addr pci_config_addr_t;

void pci_init();
uint32_t pci_read(pci_config_addr_t* config_addr);
void pci_write(pci_config_addr_t* config_addr, uint32_t data);

#endif  // __KERNEL_PCI_H__
