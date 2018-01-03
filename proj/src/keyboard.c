#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "keyboard.h"
#include "i8042.h"

//Global variables
int g_kbd_hook_id = 3; //Keyboard's hook_id
extern unsigned char g_kbd_data;
static int hook_id_bit;

//Functions

int kbd_subscribe_int()
{
	//Check hook_id range
    if(g_kbd_hook_id > 31 || g_kbd_hook_id < 0)
    	return -1;

    //Save return value (bitmask) before changing hook_id
    hook_id_bit = BIT(g_kbd_hook_id);

    //Subscribe interrupt
    if(sys_irqsetpolicy(KBD_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE),
    		&g_kbd_hook_id) != OK)
    	return -1;

    return hook_id_bit;
}

int kbd_unsubscribe_int()
{
    //Unsubscribe interrupt
    if(sys_irqrmpolicy(&g_kbd_hook_id) != OK)
    	return -1;

    return 0;
}

int get_keyboard_hook_id()
{
	return hook_id_bit;
}

int kbd_c_wrapper(unsigned int *type)
{
	//Reads scancodes from the keyboard
	kbd_c_handler();

	//If nothing was read, then do nothing
	if(g_kbd_data == 0)
		return -1;

	//Verifies if it is reading a two byte scancode
	if(g_kbd_data == TWO_BYTE)
		*type = 1;

	return 0;
}

void kbd_c_handler()
{
	unsigned long data1, stat;
	int try_cnt = 0; //Cicle gives up after a certain number of tries

	while(try_cnt < 5) /*loop while 8042 output buffer is empty*/
	{
		sys_inb(STAT_REG, &stat);

		if(stat & OBF)
		{
			sys_inb(OUT_BUF, &data1);

			//Verifies if there's no invalid data
			if ((stat & (PAR_ERR | TO_ERR)) == 0)
			{
				g_kbd_data = (unsigned char) data1;
				return;
			}
			else
			{
				g_kbd_data = 0; //0x00 is the error code
				return;
			}
		}

		try_cnt++;
		tickdelay(micros_to_ticks(DELAY_US));
	}

	clear_kbd_data();
}

void clear_kbd_data()
{
	g_kbd_data=0;
}
