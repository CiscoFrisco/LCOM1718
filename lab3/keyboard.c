#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "keyboard.h"
#include "i8042.h"

//Global variables
int g_kbd_hook_id = 3; //Keyboard's hook_id
int g_call_cnt; //Counts sys_inb() calls

//Functions

int sys_inb_cnt(port_t port, unsigned long *byte)
{
	sys_inb(port, byte);
	g_call_cnt++;

	return 0;
}

int kbd_subscribe_int()
{
	//Check hook_id range
    if(g_kbd_hook_id > 31 || g_kbd_hook_id < 0)
    	return -1;

    //Save return value (bitmask) before changing hook_id
    int hook_id_bit = BIT(g_kbd_hook_id);

    //Subscribe interrupt
    if(sys_irqsetpolicy(KBD_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &g_kbd_hook_id) != OK)
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

int kbd_c_wrapper(unsigned int *type)
{
	//Reads scancodes from the keyboard
	kbd_c_handler();

	//If nothing was read, then do nothing
	if(g_data == 0)
		return -1;

	//Verifies if it is reading a two byte scancode
	if(g_data == TWO_BYTE)
		*type = 1;

	return 0;
}


int kbd_ass_wrapper(unsigned int *type)
{
	//Reads scancodes from the keyboard
	assemblyHandler();

	//If nothing was read, then do nothing
	if(g_data == 0)
		return -1;

	//Verifies if it is reading a two byte scancode
	if(g_data == TWO_BYTE)
		*type = 1;

	return 0;
}


int print_code(unsigned int *type)
{
	//If the handler read 0xE0, then do nothing for now
	if(g_data == TWO_BYTE)
		return 0;

	//If the handler read the second byte of a two byte scancode
	if(*type == 1)
	{
		//Verifies whether *data is a breakcode or not and prints accordingly
		if(g_data & MSB)
			printf("BreakCode: 0x%02X 0x%02X\n",TWO_BYTE, g_data);
		else
			printf("MakeCode: 0x%02X 0x%02X\n",TWO_BYTE, g_data);

		//Signals that a two byte scancode has been completely processed
		*type=0;
	}

	//If the handler read a normal scancode
	else
	{
		//Verifies whether data is a breakcode or not and prints accordingly
		if(g_data & MSB)
			printf("BreakCode: 0x%02X\n", g_data);
		else
			printf("MakeCode: 0x%02X\n", g_data);
	}

	return 0;
}

void kbd_c_handler()
{
	unsigned long data1, stat;
	int try_cnt=0; //Cicle gives up after a certain number of tries

	while(try_cnt<5) /*loop while 8042 output buffer is empty*/
	{
		sys_inb_cnt(STAT_REG, &stat);

		if( stat & OBF)
		{
			sys_inb_cnt(OUT_BUF, &data1);

			//Verifies if there's no invalid data
			if ( (stat &(PAR_ERR | TO_ERR)) == 0 ){
				g_data = (unsigned char) data1;
				return;
			}
			else{
				g_data= 0; //0x00 is the error code
				return;
			}
		}

		try_cnt++;
		tickdelay(micros_to_ticks(DELAY_US));
	}
	g_data = 0;
		return;
}

int issue_comm_byte(unsigned char cmd)
{
	unsigned long stat;
	int try_cnt = 0; //Cicle gives up after a certain number of tries

	while( try_cnt < 5) {
		sys_inb(STAT_REG, &stat); /*assuming it returns OK*/
		/*loop while 8042 input buffer is not empty*/
		if( (stat & IBF) == 0 ) {
			sys_outb(KBC_CMD_REG, cmd); /*no args command*/
			return 0;
		}

		tickdelay(micros_to_ticks(DELAY_US));
		try_cnt++;
	}

	return -1;
}

void read_comm_byte(unsigned char *comm)
{
	unsigned long comm1;

	//Reads command byte to comm1
	sys_inb(OUT_BUF,&comm1);

	//Stores value in the correct format
	*comm = (unsigned char) comm1;

}

void write_comm_byte(unsigned char comm)
{
	//Writes command byte
	sys_outb(OUT_BUF,comm);
}

int enable_interrupt()
{
	unsigned char comm;

	issue_comm_byte(READ_COMM); //Signals it will read the comm byte
	read_comm_byte(&comm); //Reads the comm byte
	issue_comm_byte(WRITE_COMM); //Signals it will write a comm byte
	write_comm_byte(comm | INT_KBC); // Writes a comm byte with bit 1 set, thus enabling interrupts

	return 0;
}
























