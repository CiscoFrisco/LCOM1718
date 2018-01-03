#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <minix/syslib.h>
#include "i8042.h"
#include "mouse.h"
#include "keyboard.h"
#include "video_gr.h"

//Global variables

int g_mouse_hook_id = 5;  			//Mouse's hook_id
unsigned char g_packet [3]; 		//Array to store each packet's bytes
extern unsigned char g_mouse_data; 	// Stores bytes read from the OUT_BUF
extern int g_handler_err; 			//To signal whether the handler read correctly or not
static int hook_id_bit;


//Functions

int mouse_subscribe_int()
{
	//Check hook_id range
	if(g_mouse_hook_id > 31 || g_mouse_hook_id < 0)
		return -1;

	//Save return value (bitmask) before changing hook_id
	hook_id_bit = BIT(g_mouse_hook_id);

	//Subscribe interrupt
	if(sys_irqsetpolicy(MOUSE_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE),
			&g_mouse_hook_id) != OK)
		return -1;


	return hook_id_bit;
}

int mouse_unsubscribe_int()
{
	//Unsubscribe interrupt
	if(sys_irqrmpolicy(&g_mouse_hook_id) != OK)
		return -1;

	return 0;
}

int get_mouse_hook_id()
{
	return hook_id_bit;
}

void mouse_handler()
{
	unsigned long data1, stat;
	int try_cnt = 0; //Cicle gives up after a certain number of tries

	while(try_cnt < 5) /*loop while 8042 output buffer is empty*/
	{
		sys_inb(STAT_REG, &stat);

		if((stat & OBF) && (stat & AUX))
		{
			sys_inb(OUT_BUF, &data1);

			//Verifies if there's no invalid data
			if ((stat & (PAR_ERR | TO_ERR)) == 0)
			{
				g_mouse_data = (unsigned char) data1;
				g_handler_err = 0;
				return;
			}
			else
			{
				g_handler_err = -1;
				return;
			}
		}

		try_cnt++;
		tickdelay(micros_to_ticks(DELAY_US));
	}

	g_handler_err = -1;
}

int mouse_wrapper(int *cnt)
{
	//Reads byte from the OUT_BUF
	mouse_assembly_handler();

	//In case of a bad read, then do nothing
	if(g_handler_err == -1)
		return -1;

	//If the first byte's bit 3 is not set, then do nothing
	else if((g_mouse_data & BIT(3)) == 0 && *cnt == 0)
		return -1;

	//Else, store byte in the packet's correct position and increment *cnt
	g_packet[*cnt] = g_mouse_data;
	*cnt=*cnt+1;

	//If the packet is full reset *cnt
	if(*cnt==3)
	{
		*cnt=0;
		return 0;
	}

	return 1;
}

int checkLDOWN()
{
	if(g_packet[0] & LB_BIT)
	{
		return 0;
	}

	return 1;
}

int get_mouse_X()
{
	int x, y;

	check_coordinates(&x,&y);

	return x;
}

int get_mouse_Y()
{
	int x, y;

	check_coordinates(&x,&y);

	return y;
}

int issue_kbc_cmd(unsigned char cmd)
{
	unsigned long stat;
	int try_cnt = 0; //Cicle gives up after a certain number of tries

	while(try_cnt < 5)
	{
		sys_inb(STAT_REG, &stat); /*assuming it returns OK*/
		/*loop while 8042 input buffer is not empty*/
		if((stat & IBF) == 0)
		{
			sys_outb(KBC_CMD_REG, cmd); /*no args command*/
			return 0;
		}

		tickdelay(micros_to_ticks(DELAY_US));
		try_cnt++;
	}

	return -1;
}

int set_stream_mode(int mode)
{
	if(mode==0)
	{
		//Enable data reporting
		issue_kbc_cmd (WR_MOUSE);
		sys_outb(IN_BUF, DATA_REPORT);

		if(read_ack_byte() != 0)
			return -1;
	}

	//Set stream mode
	issue_kbc_cmd(WR_MOUSE);
	sys_outb(IN_BUF, STREAM_MODE);

	if(read_ack_byte() != 0)
		return -1;

	return 0;
}

int disable_data_reporting(int mode)
{
	issue_kbc_cmd(WR_MOUSE);
	sys_outb(IN_BUF, DISABLE);

	if(read_ack_byte() != 0)
		return -1;

	if(mode==0)
	{
		//Set remote mode
		issue_kbc_cmd (WR_MOUSE);
		sys_outb(IN_BUF, RM_MODE);

		if(read_ack_byte() != 0)
			return -1;
	}

	return 0;
}

int read_ack_byte()
{
	unsigned long ack_byte;
	unsigned long stat;

	while(1)
	{
		if(sys_inb(STAT_REG, &stat) != 0)
			return -1;

		if(stat & OBF)
			sys_inb(OUT_BUF, &ack_byte);

		//Checks acknowledgment byte
		if(ack_byte == ACK)
			return 0;
		else if (ack_byte == ERROR)
			return -1;
		else
			continue;
	}
}

void check_coordinates(int *x, int *y)
{
	*x = (int) g_packet[1];
	*y = (int) g_packet[2];

	//Check if X is negative
	if(g_packet[0] & X_SIGN)
		*x = *x | 0xFFFFFF00;

	//Check if Y is negative
	if(g_packet[0] & Y_SIGN)
		*y = *y | 0xFFFFFF00;
}

void check_mouse_boundaries(int *x, int *y){

	if(*x > (int) get_hor_resolution())
		*x = (int) get_hor_resolution();
	else if(*x < 0)
		*x = 0;

	if(*y > (int) get_ver_resolution())
		*y = (int) get_ver_resolution();
	else if(*y < 0)
		*y = 0;
}

