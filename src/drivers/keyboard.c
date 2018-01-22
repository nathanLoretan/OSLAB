/*
*   Keyboard
*/

#include <drivers/keyboard.h>

#define NBR_KEYS 256

static bool_t key_isDown[NBR_KEYS];

static void on_keyDown(char* key)
{
    printf(key);
}

static void on_keyUp(char* key)
{
    printf("KEYUP %x\n", key);
}

void kb_handler()
{
    uint8_t key_id = io_read8(PORT_PS2_DATA);

    // (key_isDown[key_id]) ? printf("on_keyDown\n") : printf("on_keyUp\n");

    // printfHex(key_isDown[key_id]);
    // printf("\n");
    // key_isDown[key_id] = !key_isDown[key_id];
    // printfHex(key_isDown[key_id]);
    // printf("\n");

    if(key_id < 0x80)
    {
        switch(key_id)
        {
            case 0x02: (key_isDown[key_id]) ? on_keyDown("1") : on_keyUp("1"); break;
            case 0x03: (key_isDown[key_id]) ? on_keyDown("2") : on_keyUp("2"); break;
            case 0x04: (key_isDown[key_id]) ? on_keyDown("3") : on_keyUp("3"); break;
            case 0x05: (key_isDown[key_id]) ? on_keyDown("4") : on_keyUp("4"); break;
            case 0x06: (key_isDown[key_id]) ? on_keyDown("5") : on_keyUp("5"); break;
            case 0x07: (key_isDown[key_id]) ? on_keyDown("6") : on_keyUp("6"); break;
            case 0x08: (key_isDown[key_id]) ? on_keyDown("7") : on_keyUp("7"); break;
            case 0x09: (key_isDown[key_id]) ? on_keyDown("8") : on_keyUp("8"); break;
            case 0x0A: (key_isDown[key_id]) ? on_keyDown("9") : on_keyUp("9"); break;
            case 0x0B: (key_isDown[key_id]) ? on_keyDown("0") : on_keyUp("0"); break;

            case 0x10: (key_isDown[key_id]) ? on_keyDown("q") : on_keyUp("q"); break;
            case 0x11: (key_isDown[key_id]) ? on_keyDown("w") : on_keyUp("w"); break;
            case 0x12: (key_isDown[key_id]) ? on_keyDown("e") : on_keyUp("e"); break;
            case 0x13: (key_isDown[key_id]) ? on_keyDown("r") : on_keyUp("r"); break;
            case 0x14: (key_isDown[key_id]) ? on_keyDown("t") : on_keyUp("t"); break;
            case 0x15: (key_isDown[key_id]) ? on_keyDown("z") : on_keyUp("z"); break;
            case 0x16: (key_isDown[key_id]) ? on_keyDown("u") : on_keyUp("u"); break;
            case 0x17: (key_isDown[key_id]) ? on_keyDown("i") : on_keyUp("i"); break;
            case 0x18: (key_isDown[key_id]) ? on_keyDown("o") : on_keyUp("o"); break;
            case 0x19: (key_isDown[key_id]) ? on_keyDown("p") : on_keyUp("p"); break;

            case 0x1E: (key_isDown[key_id]) ? on_keyDown("a") : on_keyUp("a"); break;
            case 0x1F: (key_isDown[key_id]) ? on_keyDown("s") : on_keyUp("s"); break;
            case 0x20: (key_isDown[key_id]) ? on_keyDown("d") : on_keyUp("d"); break;
            case 0x21: (key_isDown[key_id]) ? on_keyDown("f") : on_keyUp("f"); break;
            case 0x22: (key_isDown[key_id]) ? on_keyDown("g") : on_keyUp("g"); break;
            case 0x23: (key_isDown[key_id]) ? on_keyDown("h") : on_keyUp("h"); break;
            case 0x24: (key_isDown[key_id]) ? on_keyDown("j") : on_keyUp("j"); break;
            case 0x25: (key_isDown[key_id]) ? on_keyDown("k") : on_keyUp("k"); break;
            case 0x26: (key_isDown[key_id]) ? on_keyDown("l") : on_keyUp("l"); break;

            case 0x2C: (key_isDown[key_id]) ? on_keyDown("y") : on_keyUp("y"); break;
            case 0x2D: (key_isDown[key_id]) ? on_keyDown("x") : on_keyUp("x"); break;
            case 0x2E: (key_isDown[key_id]) ? on_keyDown("c") : on_keyUp("c"); break;
            case 0x2F: (key_isDown[key_id]) ? on_keyDown("v") : on_keyUp("v"); break;
            case 0x30: (key_isDown[key_id]) ? on_keyDown("b") : on_keyUp("b"); break;
            case 0x31: (key_isDown[key_id]) ? on_keyDown("n") : on_keyUp("n"); break;
            case 0x32: (key_isDown[key_id]) ? on_keyDown("m") : on_keyUp("m"); break;
            case 0x33: (key_isDown[key_id]) ? on_keyDown(",") : on_keyUp(","); break;
            case 0x34: (key_isDown[key_id]) ? on_keyDown(".") : on_keyUp("."); break;
            case 0x35: (key_isDown[key_id]) ? on_keyDown("-") : on_keyUp("-"); break;

            case 0x1C: (key_isDown[key_id]) ? on_keyDown("\n") : on_keyUp("\n"); break;
            case 0x39: (key_isDown[key_id]) ? on_keyDown(" ")  : on_keyUp(" "); break;

            default:
            {
                printf("KEYBOARD %8x\n", key_id);
                break;
            }
        }
    }
}

void kb_init()
{
    while(io_read8(PORT_PS2_CMD) & 0x1) {
        io_read8(PORT_PS2_DATA);
    }

    io_write8(PORT_PS2_CMD, 0xae); // activate interrupts
    io_write8(PORT_PS2_CMD, 0x20); // command 0x20 = read controller command byte

    uint8_t status = (io_read8(PORT_PS2_DATA) | 1) & ~0x10;

    io_write8(PORT_PS2_CMD,  0x60); // command 0x60 = set controller command byte
    io_write8(PORT_PS2_DATA, status);
    io_write8(PORT_PS2_DATA, 0xf4);

    for(uint16_t i = 0; i < NBR_KEYS; i++) {
        key_isDown[i] = TRUE;
    }

    im_subscribe(kb_handler, IRQ_KEYBOARD);
}
