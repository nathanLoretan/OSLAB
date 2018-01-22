
#include <drivers/drivers.h>


list_t drivers_list;
static size_t drivers_cnt = 0;

void drivers_init()
{
    list_init(&drivers_list, sizeof(device_t));
}

device_t* drivers_getDevice(uint8_t class_code, uint8_t subclass, uint8_t prog_if)
{
    for(device_t* it  = (device_t*)list_begin(&drivers_list);
                  it != (device_t*)list_end(&drivers_list);
                  it  = (device_t*)list_next(&drivers_list))
    {
        if(it->config_data.class_code == class_code &&
           it->config_data.subclass   == subclass   &&
           it->config_data.prog_if    == prog_if) {
            return it;
        }
    }

    return NULL;
}

int drivers_store(device_t* dev)
{
    int error = list_add(&drivers_list, dev->config_data.device_id, (uint8_t*)dev, TRUE);

    if(error == 0) {
        drivers_cnt++;
    }

    return error;
}
