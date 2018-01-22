/*
*   Drivers Manager
*/

#ifndef __DRIVERS_DRIVERS_H__
#define __DRIVERS_DRIVERS_H__

#include <common/types.h>
#include <common/list.h>
#include <drivers/pci.h>

typedef struct device {
    pci_config_data_t config_data;
    pci_config_addr_t config_addr;
}device_t;

void drivers_init();
device_t* drivers_getDevice(uint8_t class_code, uint8_t subclass, uint8_t prog_if);
int drivers_store(device_t* dev);

#endif  // __DRIVERS_DRIVERS_H__
