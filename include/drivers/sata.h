/*
*   SATA Hard Drives
*/

#ifndef __DRIVERS_SATA_H__
#define __DRIVERS_SATA_H__

#include <lib/stdio.h>
#include <common/types.h>
#include <drivers/pci.h>
#include <drivers/ata.h>
#include <drivers/drivers.h>
#include <kernel/memory/paging.h>

int sata_init();
int sata_read28(uint8_t port_nbr, uint32_t lba, uint32_t sectorCnt, uint16_t* data);
int sata_write28(uint8_t port_nbr, uint32_t lba, uint32_t sectorCnt, uint16_t* data);
void sata_clearInterrupt();

#endif // __DRIVERS_SATA_H__
