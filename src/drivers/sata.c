/*
* 	SATA Hard Drives
*
* https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/serial-ata-ahci-spec-rev1-3-1.pdf
* https://www.mindshare.com/files/ebooks/SATA%20Storage%20Technology.pdf
*
* /!\ Important, SATA uses memory mapped hardware register. Hence, it is
* important to DISABLE PAGING when handling with SATA driver.
*
* In the documentation of SATA, DW = Double word = 32bits, Word = 16bits
*
*/

#include <drivers/sata.h>

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
#define ATA_STATUS_DRQ        0x08
#define ATA_STATUS_SRV        0x10
#define ATA_STATUS_DF         0x20
#define ATA_STATUS_RDY        0x40
#define ATA_STATUS_BSY        0x80

#define AHCI_CLASS_CODE 0x01
#define AHCI_SUBCLASS   0x06
#define AHCI_PROG_IF    0x01

#define SATA_NBR_PORT		32	// Number of port
#define SATA_NBR_FIS_STR	32	// Number of FIS Structure per port
#define SATA_NBR_CMD_HDR	32	// Number of command header per command list
#define SATA_NBR_CMD_SLOT	32	// Number of command list per port

#define PRDT_NBR_ENTRY	8

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM		0x96690101	// Port multiplier

#define AHCI_DEV_NULL 		0x00
#define AHCI_DEV_SATA 		0x01
#define AHCI_DEV_SEMB 		0x02
#define AHCI_DEV_PM 		0x03
#define AHCI_DEV_SATAPI 	0x04

#define HBA_PORT_IPM_ACTIVE 	0x01
#define HBA_PORT_DET_PRESENT 	0x03

#define HBA_PxCMD_ST    0x00000001	// Start
#define HBA_PxCMD_SUD   0x00000002	// Spin up device
#define HBA_PxCMD_POD   0x00000004	// Power On Device
#define HBA_PxCMD_FRE   0x00000010	// FIS Receive Enable
#define HBA_PxCMD_FR    0x00004000	// FIS Receive Running
#define HBA_PxCMD_CR    0x00008000	// Command List Running

#define HBA_PxSCTL_DET  0x0000000F	// Device Detection Initialization

#define HBA_PxFBS_EN	0x00000001	// Enable

#define HBA_PxSERR_DIAG_X 0x04000000 // Exchanged

#define FIS_TYPE_REG_H2D	0x27	// Register FIS  		Host to Device
#define FIS_TYPE_REG_D2H	0x34	// Register FIS  		Device to Host
#define FIS_TYPE_DMA_ACT	0x39	// DMA activate 	 	Device to Host
#define FIS_TYPE_DMA_SETUP	0x41	// DMA setup  	 		Bidirectional
#define FIS_TYPE_DATA		0x46	// Data   				Bidirectional
#define FIS_TYPE_BIST		0x58	// BIST activate   	 	Bidirectional
#define FIS_TYPE_PIO_SETUP	0x5F	// PIO setup  		 	Device to Host
#define FIS_TYPE_DEV_BITS	0xA1	// Set device bits    	Device to Host

// Global HBA Control
#define HBA_GHC_HR		0x00000001 // HBA Reset
#define HBA_GHC_MRSM	0x00000003 // MSI Revert to Single Message
#define HBA_GHC_IE		0x00000002 // Interrupt Enable
#define HBA_GHC_AE		0x80000000 // AHCI Enable

// Port x Interrupt Status
#define HBA_PxIS_TFES 0x40000000 // Task File Error Status

typedef volatile struct fis_received_struct
{
	uint8_t dfis[0x1C]; 			// 0x00:0x1B DMA Setup FIS
	uint8_t reserved0[0x20-0x1C];	// 0x1C:0x19
	uint8_t psfis[0x34-0x20]; 		// 0x20:0x33 PIO Setup FIS
	uint8_t reserved1[0x40-0x34]; 	// 0x34:0x39
	uint8_t rfis[0x54-0x40]; 		// 0x40:0x53 D2H Register FIS
	uint8_t reserved2[0x58-0x54]; 	// 0x54:0x57
	uint8_t sdbfis[0x60-0x58]; 		// 0x58:0x59 Set Device Bits FIS
	uint8_t ufis[0xA0-0x60]; 		// 0x60:0x9F Unknown FIS
	uint8_t reserved3[0x100-0xA0]; 	// 0xA0:0xFF
} fis_received_struct_t;

typedef volatile struct fis_reg_h2d
{
	// DWORD 0
	uint8_t  fis_type;		// = 0x27

	uint8_t  pmport	   :4;	// Port multiplier
	uint8_t  reserved0 :3;	// Reserved
	uint8_t  c		   :1;	// 1: Command, 0: Control

	uint8_t  command;		// Command register
	uint8_t  featurel;		// Feature register, 7:0

	// DW 1
	uint8_t  lba0;			// LBA low register, 7:0
	uint8_t  lba1;			// LBA mid register, 15:8
	uint8_t  lba2;			// LBA high register, 23:16
	uint8_t  device;		// Device register

	// DW 2
	uint8_t  lba3;			// LBA register, 31:24
	uint8_t  lba4;			// LBA register, 39:32
	uint8_t  lba5;			// LBA register, 47:40
	uint8_t  featureh;		// Feature register, 15:8

	// DW 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  icc;			// Isochronous command completion
	uint8_t  control;		// Control register

	// DW 4
	uint8_t  reserved1[4];	// Reserved
} fis_reg_h2d_t;

typedef struct fis_reg_d2h
{
	// DWORD 0
	uint8_t  fis_type;    // FIS_TYPE_REG_D2H

	uint8_t  pmport:4;    // Port multiplier
	uint8_t  rsv0:2;      // Reserved
	uint8_t  i:1;         // Interrupt bit
	uint8_t  rsv1:1;      // Reserved

	uint8_t  status;      // Status register
	uint8_t  error;       // Error register

	// DWORD 1
	uint8_t  lba0;        // LBA low register, 7:0
	uint8_t  lba1;        // LBA mid register, 15:8
	uint8_t  lba2;        // LBA high register, 23:16
	uint8_t  device;      // Device register

	// DWORD 2
	uint8_t  lba3;        // LBA register, 31:24
	uint8_t  lba4;        // LBA register, 39:32
	uint8_t  lba5;        // LBA register, 47:40
	uint8_t  rsv2;        // Reserved

	// DWORD 3
	uint8_t  countl;      // Count register, 7:0
	uint8_t  counth;      // Count register, 15:8
	uint8_t  rsv3[2];     // Reserved

	// DWORD 4
	uint8_t  rsv4[4];     // Reserved
} fis_reg_d2h_t;

typedef struct fis_data
{
	// DWORD 0
	uint8_t  fis_type;	// FIS_TYPE_DATA

	uint8_t  pmport:4;	// Port multiplier
	uint8_t  rsv0:4;		// Reserved

	uint8_t  rsv1[2];	// Reserved

	// DWORD 1 ~ N
	uint32_t data[1];	// Payload
} fis_data_t;

typedef struct fis_pio_setup
{
	// DWORD 0
	uint8_t  fis_type;	// FIS_TYPE_PIO_SETUP

	uint8_t  pmport:4;	// Port multiplier
	uint8_t  rsv0:1;		// Reserved
	uint8_t  d:1;		// Data transfer direction, 1 - device to host
	uint8_t  i:1;		// Interrupt bit
	uint8_t  rsv1:1;

	uint8_t  status;	// Status register
	uint8_t  error;		// Error register

	// DWORD 1
	uint8_t  lba0;		// LBA low register, 7:0
	uint8_t  lba1;		// LBA mid register, 15:8
	uint8_t  lba2;		// LBA high register, 23:16
	uint8_t  device;		// Device register

	// DWORD 2
	uint8_t  lba3;		// LBA register, 31:24
	uint8_t  lba4;		// LBA register, 39:32
	uint8_t  lba5;		// LBA register, 47:40
	uint8_t  rsv2;		// Reserved

	// DWORD 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  rsv3;		// Reserved
	uint8_t  e_status;	// New value of status register

	// DWORD 4
	uint16_t tc;		// Transfer count
	uint8_t  rsv4[2];	// Reserved
} fis_pio_setup_t;

typedef struct fis_dma_setup
{
	// DWORD 0
	uint8_t  fis_type;	// FIS_TYPE_DMA_SETUP

	uint8_t  pmport:4;	// Port multiplier
	uint8_t  rsv0:1;		// Reserved
	uint8_t  d:1;		// Data transfer direction, 1 - device to host
	uint8_t  i:1;		// Interrupt bit
	uint8_t  a:1;            // Auto-activate. Specifies if DMA Activate FIS is needed

    uint8_t  rsved[2];       // Reserved

	//DWORD 1&2
    uint64_t DMAbufferID;    // DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.

    //DWORD 3
    uint32_t rsvd;           //More reserved

    //DWORD 4
    uint32_t DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0

    //DWORD 5
    uint32_t TransferCount;  //Number of bytes to transfer. Bit 0 must be 0

    //DWORD 6
    uint32_t resvd;          //Reserved

} fis_dma_setup_t;

typedef struct hba_prdtEntry
{
	// DW0, Data are word aligned, bit 0 reserved
	uint32_t dba;			// Data base address

	// DW1
	uint32_t dbau;			// Data base address upper 32 bits

	// DW2
	uint32_t reserved0;		// Reserved

	// DW3
	uint32_t dbc		:22;	// Byte count, 4M max
	uint32_t reserved1	:9;		// Reserved
	uint32_t i			:1; 	// Interrupt on completion
} hba_prdtEntry_t;

typedef struct hba_commandTable
{
	uint8_t cfis[0x40]; 				  // 0x00:0x39 Command FIS
	uint8_t acmd[0x50-0x40]; 			  // 0x40:0x49 ATAPI Command
	uint8_t reserved[0x80-0x50]; 		  // 0x50:0x79
	hba_prdtEntry_t prdt[PRDT_NBR_ENTRY]; // 0x80:... Physical Region Descriptor Table (0 to 65535)// TODO: define correctly the size
} hba_cmdTable_t;

typedef struct hba_cmdHeader
{
	// DW0
	uint8_t  cfl		:5;		// Command FIS length in DW (32bits)
	uint8_t  a			:1;		// ATAPI
	uint8_t  w			:1;		// Write, 1: H2D, 0: D2H
	uint8_t  p			:1;		// Prefetchable
	uint8_t  r			:1;		// Reset
	uint8_t  b			:1;		// BIST
	uint8_t  c			:1;		// Clear busy upon R_OK
	uint8_t  reserved1  :1;		// Reserved
	uint8_t  pmp		:4;		// Port multiplier port
	uint16_t prdtl;				// Physical region descriptor table length in entries

	// DW1
	uint32_t prdbc;		// Physical region descriptor byte count transferred

	// DW2, 3
	uint32_t ctba;		// Command table descriptor base address
	uint32_t ctbau;		// Command table descriptor base address upper 32 bits

	// DW4 - 7
	uint32_t reserved[4];	// Reserved
} hba_cmdHeader_t;

typedef volatile struct hba_port
{
	uint32_t clb;			// 0x00, command list base address, 1K-byte aligned
	uint32_t clbu;			// 0x04, command list base address upper 32 bits
	uint32_t fb;			// 0x08, FIS base address, 256-byte aligned
	uint32_t fbu;			// 0x0C, FIS base address upper 32 bits
	uint32_t is;			// 0x10, interrupt status
	uint32_t ie;			// 0x14, interrupt enable
	uint32_t cmd;			// 0x18, command and status
	uint32_t reserved0;		// 0x1C, Reserved
	uint32_t tfd;			// 0x20, task file data
	uint32_t sig;			// 0x24, signature
	uint32_t ssts;			// 0x28, SATA status (SCR0:SStatus)
	uint32_t sctl;			// 0x2C, SATA control (SCR2:SControl)
	uint32_t serr;			// 0x30, SATA error (SCR1:SError)
	uint32_t sact;			// 0x34, SATA active (SCR3:SActive)
	uint32_t ci;			// 0x38, command issue
	uint32_t sntf;			// 0x3C, SATA notification (SCR4:SNotification)
	uint32_t fbs;			// 0x40, FIS-based switch control
	uint32_t devslp;		// 0x44, Port Device Sleep
	uint32_t reserved1[10];	// 0x48 ~ 0x6F, Reserved
	uint32_t vendor[4];		// 0x70 ~ 0x7F, vendor specific
} hba_port_t;

typedef volatile struct hba_mem
{
	// 0x00 - 0x2B, Generic Host Control
	uint32_t cap;			// 0x00, Host capability
	uint32_t ghc;			// 0x04, Global host control
	uint32_t is;			// 0x08, Interrupt status
	uint32_t pi;			// 0x0C, Port implemented
	uint32_t vs;			// 0x10, Version
	uint32_t ccc_ctl;		// 0x14, Command completion coalescing control
	uint32_t ccc_pts;		// 0x18, Command completion coalescing ports
	uint32_t em_loc;		// 0x1C, Enclosure management location
	uint32_t em_ctl;		// 0x20, Enclosure management control
	uint32_t cap2;			// 0x24, Host capabilities extended
	uint32_t bohc;			// 0x28, BIOS/OS handoff control and status

	// 0x2C - 0x9F, Reserved
	uint8_t  reserved[0xA0-0x2C];

	// 0xA0 - 0xFF, Vendor specific registers
	uint8_t  vendor[0x100-0xA0];

	// 0x100 - 0x10FF, Port control registers
	hba_port_t	ports[SATA_NBR_PORT];
} hba_mem_t;

device_t* sata = NULL;
hba_mem_t* abar = NULL;

uint8_t ahci_getFreeSlot(hba_port_t* port)
{
	// Each bit of sact(command is outstanding) or ci(command my be sent)
	// indicates if a command slot is used if they are set

	uint32_t slots = (port->sact | port->ci);
	for(uint8_t i = 0; i < SATA_NBR_CMD_SLOT; i++)
	{
		if((slots & 1) == 0) {
			return i;
		}

		slots >>= 1;  // Get the next bit corresponding to the next port
	}

	return -1;
}

void ahci_enableCmd(hba_port_t* port)
{
	// Wait if command list DMA engine for port is running
	while(port->cmd & HBA_PxCMD_CR){}

	while(port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)){}
	port->cmd |= HBA_PxCMD_FRE; // FISes are accepted
	port->fbs |= HBA_PxFBS_EN;  // Process the command list
	port->cmd |= HBA_PxCMD_ST;  // Process the command list

	// Enter in state P:Idle, PxCMD.CR = 1
	while(!(port->cmd & HBA_PxCMD_CR)){}
}

void ahci_DisableCmd(hba_port_t* port)
{
	port->cmd &= ~HBA_PxCMD_ST;	 // Not process the command list
	port->cmd &= ~HBA_PxCMD_FRE; // FISes are not accepted

	// Wait if command list DMA or FIS receive DMA engine for port is running
	while((port->cmd & HBA_PxCMD_FR) || (port->cmd & HBA_PxCMD_CR)){}
}

void ahci_portInit(hba_port_t* port, uint8_t port_nbr)
{
	#define	AHCI_BASE 0x01000000	// AHCI base at memory address

	// Power On the port if not already done
	if(!(port->cmd & HBA_PxCMD_POD)) {
		port->cmd |= HBA_PxCMD_POD;
	}

	ahci_DisableCmd(port);	// Stop command engine

	// The memory space of each port is built with 32 (command list (1k aligned)
	// + receive FIS structure (256bytes aligned)), Each command list has 32
	// Command header. Each command header has a command table

	// Set Command List memory space
	port->clb  = AHCI_BASE + (port_nbr * 1024); // Command list, 1kbyte aligned
	port->clbu = 0;
	memset((void*)port->clb, 0, 1024);

	// Set FIS memory space for receive area
	port->fb  = AHCI_BASE + (SATA_NBR_CMD_SLOT * 1024) + (port_nbr * 256); // FIS, 256byte aligned
	port->fbu = 0;
	memset((void*)port->fb, 0, 256);

	// The command table is at 40k of the memory space because before there
	// are 32*1k (cmd list) + 32*256 (FIS struct) = 40960.
	// With 8 PRDT entries (8*16 = 128bytes), the size of the command table
	// is 256bytes. The size of all the command tables of one port represents
	// 32 (nbr cmd hdr) * 256 (size of one cmd table) = 8192

	// Set Command Header memory space
	hba_cmdHeader_t* cmdheader = (hba_cmdHeader_t*)(port->clb);
	for(uint8_t i = 0; i < SATA_NBR_CMD_HDR; i++)
	{
		cmdheader[i].prdtl = PRDT_NBR_ENTRY;

		cmdheader[i].ctba  = AHCI_BASE + (SATA_NBR_CMD_SLOT * 1024) +
							(SATA_NBR_FIS_STR * 256) + (i * 256) +
							(port_nbr * 8192);
		cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}

	// Enable all the interrupt of the port
	port->is = 0xFFFFFFFF; // Clear pending interrupt
	port->ie = 0xFFFFFFFF;

	ahci_enableCmd(port);	// Start command engine
}

uint8_t ahci_getDeviceType(hba_port_t* port)
{
	// Device Detection
	if ((port->ssts & 0x0F) != HBA_PORT_DET_PRESENT) {
		return AHCI_DEV_NULL;
	}

	// Interface Power Management
	if (((port->ssts >> 8) & 0x0F) != HBA_PORT_IPM_ACTIVE) {
		return AHCI_DEV_NULL;
	}

	uint8_t sig; // Port Signature
	switch(port->sig)
	{
		case SATA_SIG_ATAPI: sig = AHCI_DEV_SATAPI;
		break;
		case SATA_SIG_SEMB:  sig = AHCI_DEV_SEMB;
		break;
		case SATA_SIG_PM: 	 sig = AHCI_DEV_PM;
		break;
		default: 			 sig = AHCI_DEV_SATA;
		break;
	}

	return sig;
}

void ahci_enableInterrupt(hba_mem_t* abar)
{
	abar->is   = 0xFFFFFFFF; // Clear pending interrupt
	abar->ghc |= HBA_GHC_IE;
}

void ahci_disableInterrupt(hba_mem_t* abar)
{
	abar->ghc &= ~HBA_GHC_IE;
}

void ahci_searchPort(hba_mem_t* abar)
{
	// Search disk in impelemented ports
	uint32_t pi = abar->pi;

	for(uint8_t i = 0; i < SATA_NBR_PORT; i++)
	{
		if(pi & 0x01)	// Test if a device is attached to the port
		{
			uint8_t type = ahci_getDeviceType(&abar->ports[i]);

			if(type == AHCI_DEV_SATA) {
				printf("SATA drive found at port %8x\n", i);
				ahci_portInit(&abar->ports[i], i);
			}
			else if(type == AHCI_DEV_SATAPI) {
				// printf("SATAPI drive found at port %8x\n", i);
			}
			else if (type == AHCI_DEV_SEMB) {
				// printf("SEMB drive found at port %8x\n", i);
			}
			else if (type == AHCI_DEV_PM) {
				// printf("PM drive found at port %8x\n", i);
			}
			else {
				// printf("No drive found at port %8x\n", i);
			}
		}
		// else {
		// 	printf("No drive found at port %8x\n", i);
		// }

		pi >>= 1; // Get the next bit corresponding to the next port
	}
}

int sata_read28(uint8_t port_nbr, uint32_t lba, uint32_t sectorCnt, uint16_t* data)
{
	// TODO timeout
	int spin = 0; // Spin lock timeout counter

	// Disable paging to use memory mapped hardware register
	paging_disable();

	hba_port_t* port = &abar->ports[port_nbr];

	uint8_t slot = ahci_getFreeSlot(port);
	if(slot == -1) {
		paging_enable();
		return -1;
	}

	// Get the command Header corresponding to the command list slot of the port
	hba_cmdHeader_t* cmdHeader = (hba_cmdHeader_t*)(port->clb + slot);
	cmdHeader->w	 = 0; 	// Read
	cmdHeader->p	 = 0;   // Prefetchable

	// Command FIS length
	cmdHeader->cfl 	 = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);

	// Get the command Table corresponding to the command Header
	hba_cmdTable_t* cmdTable = (hba_cmdTable_t*) cmdHeader->ctba;
	memset(cmdTable, 0, sizeof(hba_cmdTable_t));

	// The data needs to be 16bits aligned, lower bit = 0
	uint16_t* ptr = data;
	uint32_t remain = sectorCnt * 512; // 1 sector = 512bits

	for(int i = 0; i < cmdHeader->prdtl - 1 && remain != 0; i++)
	{
		// If the data if bigger than 4MB
		uint32_t dbc = (remain > (4*1024*1024-1)) ? (4*1024*1024)-1 : remain;

		// prdt.dba -> Physical Address of Data Block
		// prdt.dbc -> Byte Count, bit 0 always = 1
		// prdt.i   -> Interrupt on Completion
		cmdTable->prdt[i].dba = (uint32_t)ptr;
		cmdTable->prdt[i].dbc = dbc;
		cmdTable->prdt[i].i   = 1;

		ptr    += dbc;
		remain -= dbc;
	}

	// Get the command FIS (address 0x00 to 0x40 of the command table)
	fis_reg_h2d_t* cmdfis = (fis_reg_h2d_t*)(&cmdTable->cfis);

	// Set the ATA command
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->command  = ATA_CMD_READ_DMA_EXT;
	cmdfis->c 		 = 1;

	cmdfis->lba0 = (uint8_t) lba;
	cmdfis->lba1 = (uint8_t)(lba >> 8);
	cmdfis->lba2 = (uint8_t)(lba >> 16);
	cmdfis->device = 0x40 | (lba >> 24);	// LBA mode

	cmdfis->countl =  sectorCnt & 0xFF;

	//=========================================================================
	// Indicate that the command may be send to the device
	// port->ci |= 1 << slot;

	// // Go to state StartComm
	// port->sctl &= ~HBA_PxSCTL_DET;
	// port->cmd  &= ~HBA_PxCMD_SUD;
	// port->sctl |= HBA_PxSCTL_DET;
	// port->cmd  |= HBA_PxCMD_SUD;
	//
	//
	// // Go to state RegFisPostToMem
	// port->serr &= ~HBA_PxSERR_DIAG_X;
	// port->cmd  &= ~HBA_PxCMD_FRE;
	// port->cmd  |= HBA_PxCMD_FRE;
	//==========================================================================

	// // Assume that port is in state P:idle
	// port->ci |= 1 << slot; // P:idle -> P:SelectCmd -> P:FetchCmd

	// Assume that port is in state P:NotRunning
	// port->sctl |= HBA_PxSCTL_DET;
	// port->cmd  |= HBA_PxCMD_SUD;

	// port->cmd |= HBA_PxCMD_ST;  // Process the command list

	// printf("%32x, %32x\n", port->cmd, HBA_PxCMD_ST);
	// printf("%32x %32x, %32x\n", port->tfd, ATA_STATUS_BSY, ATA_STATUS_DRQ);

	// Assume that port is in state FB:idle
	port->ci |= 1 << slot; // FB:SelectDevice -> SelectCmd -> FetchCmd ->idle

	// Poll the command port until BSY and DRQ bit aer clear or timeout
	while((port->tfd & ATA_STATUS_BSY) && spin < 1000000) {}

	// printf("%32x\n", port->cmd & HBA_PxCMD_CR);

	// // Poll the command port until BSY and DRQ bit aer clear or timeout
	// while((port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) &&
	// 	 !(port->tfd & ATA_STATUS_ERR) && spin < 1000000)
	// {
	// 	printf("%32x\n", spin++);
	// }
	//
	// if(port->tfd & ATA_STATUS_ERR) {
	// 	paging_enable();
	// 	return -1;
	// }

	// If timeout
	if (spin >= 1000000)
	{
		paging_enable();
		return -2;
	}

	//  port->ci |= 1 << slot;

	// HBA clears PxCi when the command in the slot is finished
	// spin = 0;
	// while((port->ci & (1 << slot)) == 0  && spin < 1000000)
	// {
	// 	if (port->is & HBA_PxIS_TFES)
	// 	{
	// 		paging_enable();
	// 		return -3;
	// 	}
	//
	// 	spin++;
	// }
	//
	// // If timeout
	// if (spin >= 1000000)
	// {
	// 	paging_enable();
	// 	return -4;
	// }
	//
	// // Check again
	// if (port->is & HBA_PxIS_TFES)
	// {
	// 	paging_enable();
	// 	return -5;
	// }

	paging_enable();
	return 0;
}

int sata_write28(uint8_t port_nbr, uint32_t lba, uint32_t sectorCnt, uint16_t* data)
{
	// TODO timeout
	int spin = 0; // Spin lock timeout counter

	// Disable paging to use memory mapped hardware register
	paging_disable();

	hba_port_t* port = &abar->ports[port_nbr];

	uint8_t slot = ahci_getFreeSlot(port);
	if(slot == -1) {
		paging_enable();
		return -1;
	}

	// Get the command Header corresponding to the command list slot of the port
	hba_cmdHeader_t* cmdHeader = (hba_cmdHeader_t*)(port->clb + slot);
	cmdHeader->w	 = 1; 	// Read
	cmdHeader->p	 = 0;   // Prefetchable

	// Command FIS length
	cmdHeader->cfl 	 = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);

	// Get the command Table corresponding to the command Header
	hba_cmdTable_t* cmdTable = (hba_cmdTable_t*) cmdHeader->ctba;
	memset(cmdTable, 0, sizeof(hba_cmdTable_t));

	// The data needs to be 16bits aligned, lower bit = 0
	uint16_t* ptr = data;
	uint32_t remain = sectorCnt * 512; // 1 sector = 512bits

	for(int i = 0; i < cmdHeader->prdtl - 1 && remain != 0; i++)
	{
		// If the data if bigger than 4MB
		uint32_t dbc = (remain > (4*1024*1024-1)) ? (4*1024*1024)-1 : remain;

		// prdt.dba -> Physical Address of Data Block
		// prdt.dbc -> Byte Count, bit 0 always = 1
		// prdt.i   -> Interrupt on Completion
		cmdTable->prdt[i].dba = (uint32_t)ptr;
		cmdTable->prdt[i].dbc = dbc;
		cmdTable->prdt[i].i   = 1;

		ptr    += dbc;
		remain -= dbc;
	}

	// Get the command FIS (address 0x00 to 0x40 of the command table)
	fis_reg_h2d_t* cmdfis = (fis_reg_h2d_t*)(&cmdTable->cfis);

	// Set the ATA command
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->command  = ATA_CMD_WRITE_DMA_EXT;
	cmdfis->c 		 = 1;

	cmdfis->lba0 = (uint8_t) lba;
	cmdfis->lba1 = (uint8_t)(lba >> 8);
	cmdfis->lba2 = (uint8_t)(lba >> 16);
	cmdfis->device = 0x40 | (lba >> 24);	// LBA mode

	cmdfis->countl =  sectorCnt & 0xFF;

	// Assume that port is in state FB:idle
	port->ci |= 1 << slot; // FB:SelectDevice -> SelectCmd -> FetchCmd ->idle

	// Poll the command port until BSY and DRQ bit aer clear or timeout
	while((port->tfd & ATA_STATUS_BSY) && spin < 1000000) {}

	// If timeout
	if (spin >= 1000000)
	{
		paging_enable();
		return -2;
	}

	paging_enable();
	return 0;
}

int sata_init()
{
    sata = drivers_getDevice(AHCI_CLASS_CODE, AHCI_SUBCLASS, AHCI_PROG_IF);

	if(sata == NULL) {
        return -1;
    }

	// Disable paging to use memory mapped hardware register
	paging_disable();

	abar = (hba_mem_t*) sata->config_data.base_addr_5;

	ahci_searchPort(abar);
	ahci_enableInterrupt(abar);

	paging_enable();

    return 0;
}
