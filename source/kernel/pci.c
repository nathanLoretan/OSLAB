/*
*   Peripheral Componenent Interconnect
*/

#include <kernel/pci.h>

static uint8_t pci_device_nbr_func(uint8_t bus, uint8_t device)
{
    pci_config_addr_t config_addr =
    {
        .offset   = PCI_OFFSET_MF,
        .function = 0,
        .device   = device,
        .bus      = bus,
    };

    return (pci_read(&config_addr) & PCI_MASK_MF) ? PCI_FUNCTION_NBR : 1;
}

static void pci_get_config_data(pci_device_t *dev)
{
    dev->config_addr.offset     = PCI_OFFSET_VENDOR_ID;
    dev->config_data.vendor_id  = pci_read(&dev->config_addr) & PCI_MASK_VENDOR_ID;

    // Test if the vendor id is valide
    if(dev->config_data.vendor_id == 0xFFFF ||
       dev->config_data.vendor_id == 0x0000) {
           return;
       }

    dev->config_addr.offset     = PCI_OFFSET_DEVICE_ID;
    dev->config_data.device_id  = pci_read(&dev->config_addr) & PCI_MASK_DEVICE_ID;

    dev->config_addr.offset     = PCI_OFFSET_REVISION;
    dev->config_data.revision   = pci_read(&dev->config_addr) & PCI_MASK_REVISION;

    dev->config_addr.offset     = PCI_OFFSET_CLASS;
    dev->config_data.class_code = pci_read(&dev->config_addr) & PCI_MASK_CLASS;

    dev->config_addr.offset     = PCI_OFFSET_SUBCLASS;
    dev->config_data.subclass   = pci_read(&dev->config_addr) & PCI_MASK_SUBCLASS;

    dev->config_addr.offset     = PCI_OFFSET_PROG_IF;
    dev->config_data.prog_if    = pci_read(&dev->config_addr) & PCI_MASK_PROG_IF;

    dev->config_addr.offset     = PCI_OFFSET_INTERRUPT;
    dev->config_data.interrupt  = pci_read(&dev->config_addr) & PCI_MASK_INTERRUPT;

    dev->config_addr.offset     = PCI_OFFSET_HEADERTYPE;
    dev->config_data.headertype = pci_read(&dev->config_addr) & PCI_MASK_HEADERTYPE;

    // Search the Base Address Register of the device
    if(dev->config_data.headertype == PCI_BAR_MEMORYSPACE)
    {
        dev->config_addr.offset       = PCI_OFFSET_BAR_0;
        dev->config_data.base_addr_0  = pci_read(&dev->config_addr) & PCI_MASK_BAR;

        dev->config_addr.offset       = PCI_OFFSET_BAR_1;
        dev->config_data.base_addr_1  = pci_read(&dev->config_addr) & PCI_MASK_BAR;

        dev->config_addr.offset       = PCI_OFFSET_BAR_2;
        dev->config_data.base_addr_2  = pci_read(&dev->config_addr) & PCI_MASK_BAR;

        dev->config_addr.offset       = PCI_OFFSET_BAR_3;
        dev->config_data.base_addr_3  = pci_read(&dev->config_addr) & PCI_MASK_BAR;

        dev->config_addr.offset       = PCI_OFFSET_BAR_4;
        dev->config_data.base_addr_4  = pci_read(&dev->config_addr) & PCI_MASK_BAR;

        dev->config_addr.offset       = PCI_OFFSET_BAR_5;
        dev->config_data.base_addr_5  = pci_read(&dev->config_addr) & PCI_MASK_BAR;
    }
    else if(dev->config_data.headertype == PCI_BAR_IOSPACE)
    {
        dev->config_addr.offset       = PCI_OFFSET_BAR_0;
        dev->config_data.base_addr_0  = pci_read(&dev->config_addr) & PCI_MASK_BAR;

        dev->config_addr.offset       = PCI_OFFSET_BAR_1;
        dev->config_data.base_addr_1  = pci_read(&dev->config_addr) & PCI_MASK_BAR;
    }
}

void pci_init()
{
    for(uint8_t bus = 0; bus < PCI_BUS_NBR; bus++)
    {
        for(uint8_t device = 0; device < PCI_DEVICE_NBR; device++)
        {
            for(uint8_t func = 0; func < pci_device_nbr_func(bus, device); func++)
            {
                pci_device_t dev =
                {
                    .config_addr =
                    {
                        .offset   = 0x00,
                        .function = func,
                        .device   = device,
                        .bus      = bus,
                    }
                };

                pci_get_config_data(&dev);

                if(dev.config_data.vendor_id == 0x0000 ||
                   dev.config_data.vendor_id == 0xFFFF)
                    continue; // Look for next functions of the device

                printf("PCI BUS ");
                printfHex(bus & 0xFF);

                printf(", DEVICE ");
                printfHex(device & 0xFF);

                printf(", FUNCTION ");
                printfHex(func & 0xFF);

                printf(" = VENDOR ");
                printfHex((dev.config_data.vendor_id & 0xFF00) >> 8);
                printfHex(dev.config_data.vendor_id & 0xFF);
                printf(", DEVICE ");
                printfHex((dev.config_data.device_id & 0xFF00) >> 8);
                printfHex(dev.config_data.device_id & 0xFF);
                printf("\n");
            }
        }
    }
}

uint32_t pci_read(pci_config_addr_t* config_addr)
{
    uint8_t offset = config_addr->offset;
    config_addr->offset = config_addr->offset & 0xFC;
    config_addr->enable = 1;

    io_write32(PORT_PCI_CMD, (uint32_t)(*(uint32_t*) config_addr));
    return io_read32(PORT_PCI_DATA) >> (8 * (offset % 4));
}

void pci_write(pci_config_addr_t* config_addr, uint32_t data)
{
    config_addr->offset = config_addr->offset & 0xFC;
    config_addr->enable = 1;

    io_write32(PORT_PCI_CMD, (uint32_t)(*(uint32_t*) config_addr));
    io_write32(PORT_PCI_DATA, data);
}
