/*
*   ATA Hard Drives
*/

#ifndef __DRIVERS_ATA_H__
#define __DRIVERS_ATA_H__

#include <common/types.h>
#include <lib/stdio.h>
#include <drivers/io.h>

#define ATA_PRIMARY_MASTER      0
#define ATA_PRIMARY_SLAVE       1
#define ATA_SECONDARY_MASTER    2
#define ATA_SECONDARY_SLAVE     3

int ata_identify(uint8_t type);
int ata_read28 (uint8_t type, uint32_t sector, uint8_t* data, size_t count);
int ata_write28(uint8_t type, uint32_t sector, uint8_t* data, size_t count);
int ata_flush(uint8_t type);

#endif // __DRIVERS_ATA_H__
