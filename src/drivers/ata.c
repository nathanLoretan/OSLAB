/*
*   ATA Hard Drives
*/

// Find out how to install ATA DMA with Virtual box

#include <drivers/ata.h>

// Advanced Technology Attachement
#define PORT_ATA_PRI           0x01F0 // Port base address of ATA Primary
#define PORT_ATA_SEC           0x0170 // Port base address of ATA Secondary

#define PORT_ATA_DATA          0x0000 // + PORT_ATA_PRI or PORT_ATA_SEC
#define PORT_ATA_ERROR         0x0001 // + PORT_ATA_PRI or PORT_ATA_SEC
#define PORT_ATA_SECTOR_CNT    0x0002 // + PORT_ATA_PRI or PORT_ATA_SEC
#define PORT_ATA_LBA0          0x0003 // + PORT_ATA_PRI or PORT_ATA_SEC
#define PORT_ATA_LBA1          0x0004 // + PORT_ATA_PRI or PORT_ATA_SEC
#define PORT_ATA_LBA2          0x0005 // + PORT_ATA_PRI or PORT_ATA_SEC
#define PORT_ATA_DRIVE         0x0006 // + PORT_ATA_PRI or PORT_ATA_SEC
#define PORT_ATA_CMD           0x0007 // + PORT_ATA_PRI or PORT_ATA_SEC
#define PORT_ATA_CTL           0x0206 // + PORT_ATA_PRI or PORT_ATA_SEC

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define ATA_STATUS_ERR        0x01
#define ATA_STATUS_DRQ        0x08  // Data request
#define ATA_STATUS_SRV        0x10
#define ATA_STATUS_DF         0x20
#define ATA_STATUS_RDY        0x40  // Ready
#define ATA_STATUS_BSY        0x80  // Busy

#define ATA_CTL_NIEN   0x01 // Stop sending interrupt
#define ATA_CTL_SRST   0x02 // Software Reset on all ATA drives
#define ATA_CTL_HOB    0x80 // Read back the High Order Byte

#define ATA_MASTER     0xE0
#define ATA_SLAVE      0xF0

#define ATA_IDENTIFY_MASTER     0xA0
#define ATA_IDENTIFY_SLAVE      0xB0

#define ATA_PIO_SECTOR_SIZE 512

volatile bool_t isAtaPriReady;
volatile bool_t isAtaSecReady;

int ata_identify(uint8_t type)
{
    uint8_t status = 0;

    uint16_t base = (type & 0x02) ? PORT_ATA_SEC       : PORT_ATA_PRI;
    uint8_t  role = (type & 0x01) ? ATA_IDENTIFY_SLAVE : ATA_IDENTIFY_MASTER;

    io_write8(base + PORT_ATA_DRIVE, role);
    io_write8(base + PORT_ATA_CTL, 0);
    io_write8(base + PORT_ATA_DRIVE, role);

    // Check if the hard drive exists
    status = io_read8(base + PORT_ATA_CMD);
    if( status == 0xFF || status == 0x00) {
        printf("ATA Drive Doesn't exist\n");
        return -1;
    }

    // Need to read the status once to clear the interrupt flag of the disk
    status = io_read8(base + PORT_ATA_CMD);
    (type & 0x02) ? (isAtaSecReady = FALSE) : (isAtaPriReady = FALSE);

    io_write8(base + PORT_ATA_DRIVE, role);
    io_write8(base + PORT_ATA_SECTOR_CNT, 0);
    io_write8(base + PORT_ATA_LBA0, 0);
    io_write8(base + PORT_ATA_LBA1, 0);
    io_write8(base + PORT_ATA_LBA2, 0);
    io_write8(base + PORT_ATA_CMD, ATA_CMD_IDENTIFY);

    // Wait the interrupt from the ATA device
    while((type & 0x02) ? !isAtaSecReady : !isAtaPriReady){} // TODO: ADD Timeout

    // Check if the hard drive exists
    status = io_read8(base + PORT_ATA_CMD);
    if( status == 0xFF || status == 0x00) {
        printf("ATA Drive Doesn't exist\n");
        return -1;
    }
    //
    // // Poll the command port until BSY bit is clears
    // while((status & ATA_STATUS_BSY) && !(status & ATA_STATUS_ERR)) {
    //     status = io_read8(base + PORT_ATA_CMD);
    // }

    if((status = io_read8(base + PORT_ATA_CMD)) & ATA_STATUS_ERR) {
        return -1;
    }

    // Display the different information get with the identification
    for(int i = 0; i < 256; i++)
    {
        uint16_t data = io_read16(base + PORT_ATA_DATA);
        // TODO: Save the information in a correct struct
    }

    return 0;
}

int ata_read28(uint8_t type, uint32_t sector, uint8_t* data, size_t count)
{
    uint8_t status = 0;

    uint8_t  role = (type & 0x01) ? ATA_SLAVE    : ATA_MASTER;
    uint16_t base = (type & 0x02) ? PORT_ATA_SEC : PORT_ATA_PRI;

    // Check if the sector has a correct value
    if(sector > 0x0FFFFFFF) {
        return -1;
    }

    // Need to read the status once to clear the interrupt flag of the disk
    status = io_read8(base + PORT_ATA_CMD);
    (type & 0x02) ? (isAtaSecReady = FALSE) : (isAtaPriReady = FALSE);

    io_write8(base + PORT_ATA_DRIVE, role  | ((sector & 0x0F000000) >> 24));
    io_write8(base + PORT_ATA_ERROR, 0);
    io_write8(base + PORT_ATA_SECTOR_CNT, 1);
    io_write8(base + PORT_ATA_LBA0,  sector & 0x000000FF);
    io_write8(base + PORT_ATA_LBA1, (sector & 0x0000FF00) >> 8);
    io_write8(base + PORT_ATA_LBA2, (sector & 0x00FF0000) >> 16);
    io_write8(base + PORT_ATA_CMD, ATA_CMD_READ_PIO);

    // Wait the interrupt from the ATA device
    while((type & 0x02) ? !isAtaSecReady : !isAtaPriReady){} // TODO: ADD Timeout

    // Check if the hard drive exists
    status = io_read8(base + PORT_ATA_CMD);
    if(status == 0x00 || status == 0xFF) {
        printf("ATA Drive Doesn't exist\n");
        return -1;
    }

    // // Poll the command port until BSY bit is clears
    // while((status & ATA_STATUS_BSY) && !(status & ATA_STATUS_ERR)) {
    //     status = io_read8(base + PORT_ATA_CMD);
    // }
    //
    if((status = io_read8(base + PORT_ATA_CMD)) & ATA_STATUS_ERR) {
        return -1;
    }

    // 512bytes should be read. Therefore, the first loop get the date wanted
    // and the second complete the lecture with the remained bytes

    size_t test = 0;

    // Read from the sector
    for(size_t i = 0; i < count; i += 2)
    {
        uint16_t buffer = io_read16(base + PORT_ATA_DATA);

        if(i+1 < count) {
            data[i]   = buffer & 0x00FF;
            data[i+1] = buffer >> 8;
        }
        else {
            data[i] = buffer & 0x00FF;
        }

        // printf("%8x %8x ", data[i], data[i+1]);
    }
    // printf("\n");

    for(size_t i = count + (count%2); i < ATA_PIO_SECTOR_SIZE; i += 2) {
        io_read16(base + PORT_ATA_DATA);
    }

    return 0;
}

int ata_write28(uint8_t type, uint32_t sector, uint8_t* data, size_t count)
{
    uint8_t status = 0;

    uint8_t  role = (type & 0x01) ? ATA_SLAVE    : ATA_MASTER;
    uint16_t base = (type & 0x02) ? PORT_ATA_SEC : PORT_ATA_PRI;

    // Check if the sector has a correct value
    if(sector > 0x0FFFFFFF || count > ATA_PIO_SECTOR_SIZE) {
        return -1;
    }

    // Need to read the status once to clear the interrupt flag of the disk
    status = io_read8(base + PORT_ATA_CMD);
    (type & 0x02) ? (isAtaSecReady = FALSE) : (isAtaPriReady = FALSE);

    io_write8(base + PORT_ATA_DRIVE, role  | ((sector & 0x0F000000) >> 24));
    io_write8(base + PORT_ATA_ERROR, 0);
    io_write8(base + PORT_ATA_SECTOR_CNT, 1);
    io_write8(base + PORT_ATA_LBA0,  sector & 0x000000FF);
    io_write8(base + PORT_ATA_LBA1, (sector & 0x0000FF00) >> 8);
    io_write8(base + PORT_ATA_LBA2, (sector & 0x00FF0000) >> 16);
    io_write8(base + PORT_ATA_CMD, ATA_CMD_WRITE_PIO);

    // Check if the hard drive exists
    status = io_read8(base + PORT_ATA_CMD);
    if(status == 0x00 || status == 0xFF) {
        printf("ATA Drive Doesn't exist\n");
        return -1;
    }

    // Poll the command port until BSY bit is clears, The host need to wait in
    // a loop because the drive doesn't send an interrupt at this step
    while((status & ATA_STATUS_BSY) && !(status & ATA_STATUS_ERR)) {
        status = io_read8(base + PORT_ATA_CMD);
    }

    // 512bytes should be write. Therefore, the first loop get the date wanted
    // and the second complete the lecture with the remained bytes

    // Write on the sector, get 256*16bits
    for(int i = 0; i < count; i += 2)
    {
        uint16_t buffer;

        if(i+1 < count) {
            buffer = data[i] | (data[i+1] << 8);
        }
        else {
            buffer = data[i];
        }

        io_write16(base + PORT_ATA_DATA, buffer);

        // printf("%8x %8x ", data[i], data[i+1]);
    }
    // printf("\n");

    for(int i = count + (count%2); i < ATA_PIO_SECTOR_SIZE; i += 2) {
        io_write16(base + PORT_ATA_DATA, 0x0000);
    }

    // Wait the interrupt from the ATA device
    while((type & 0x02) ? !isAtaSecReady : !isAtaPriReady){} // TODO: ADD Timeout

    // Check if the hard drive exists
    status = io_read8(base + PORT_ATA_CMD);
    if(status == 0x00 || status == 0xFF) {
        printf("ATA Drive Doesn't exist\n");
        return -1;
    }

    if((status = io_read8(base + PORT_ATA_CMD)) & ATA_STATUS_ERR) {
        return -1;
    }

    return 0;
}

int ata_flush(uint8_t type)
{
    uint8_t status = 0;

    uint8_t  role = (type & 0x01) ? ATA_SLAVE    : ATA_MASTER;
    uint16_t base = (type & 0x02) ? PORT_ATA_SEC : PORT_ATA_PRI;

    // Need to read the status once to clear the interrupt flag of the disk
    status = io_read8(base + PORT_ATA_CMD);
    (type & 0x02) ? (isAtaSecReady = FALSE) : (isAtaPriReady = FALSE);

    io_write8(base + PORT_ATA_DRIVE, role);
    io_write8(base + PORT_ATA_CMD, ATA_CMD_CACHE_FLUSH);

    // Wait the interrupt from the ATA device
    while((type & 0x02) ? !isAtaSecReady : !isAtaPriReady){} // TODO: ADD Timeout

    // Check if the hard drive exists
    status = io_read8(base + PORT_ATA_CMD);
    if(status == 0x00 || status == 0xFF) {
        printf("ATA Drive Doesn't exist\n");
        return -1;
    }

    // // Poll the command port until BSY bit is clears
    // while((status & ATA_STATUS_BSY) && !(status & ATA_STATUS_ERR)) {
    //     status = io_read8(base + PORT_ATA_CMD);
    // }

    if((status = io_read8(base + PORT_ATA_CMD)) & ATA_STATUS_ERR) {
        return -1;
    }

    return 0;
}
