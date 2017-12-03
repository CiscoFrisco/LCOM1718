#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include "mouse.h"

//Global variables

int g_mouse_hook_id = 5;  //Mouse's hook_id
int g_kbd_hook_id = 3; //Keyboard's hook_id
unsigned char g_packet [3]; //Array to store each packet's bytes
unsigned char g_data; // Stores bytes read from the OUT_BUF
int g_handler_err = 0; //To signal whether the handler read correctly or not


//Functions

int mouse_subscribe_int()
{
	//Check hook_id range
	if(g_mouse_hook_id > 31 || g_mouse_hook_id < 0)
		return -1;

	//Save return value (bitmask) before changing hook_id
	int hook_id_bit = BIT(g_mouse_hook_id);

	//Subscribe interrupt
	if(sys_irqsetpolicy(MOUSE_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &g_mouse_hook_id) != OK)
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

void mouse_handler()
{
	unsigned long data1, stat;
	int try_cnt=0; //Cicle gives up after a certain number of tries

	while(try_cnt<5) /*loop while 8042 output buffer is empty*/
	{
		sys_inb(STAT_REG, &stat);

		if((stat & OBF) && (stat & AUX))
		{
			sys_inb(OUT_BUF, &data1);

			//Verifies if there's no invalid data
			if ( (stat &(PAR_ERR | TO_ERR)) == 0 ){

				g_data = (unsigned char) data1;
				g_handler_err = 0;
				return;

			}
			else{
				g_handler_err = -1;
				return;
			}
		}

		try_cnt++;
		tickdelay(micros_to_ticks(DELAY_US));
	}

	g_handler_err = -1;
	return;
}

int mouse_wrapper(int *cnt)
{
	//Reads byte from the OUT_BUF
	mouse_handler();

	//In case of a bad read, then do nothing
	if(g_handler_err == -1)
		return -1;

	//If the first byte's bit 3 is not set, then do nothing
	else if((g_data & BIT(3)) == 0 && *cnt == 0)
		return -1;

	//Else, store byte in the packet's correct position and increment *cnt
	g_packet[*cnt] = g_data;
	*cnt=*cnt+1;

	//If the packet is full, then print it. Afterwards, reset *cnt and the packet's values
	if(*cnt==3){
		*cnt=0;
		return 0;
	}

	return 1;
}


void print_packet()
{
	int x, y;
	unsigned char byte1=g_packet[0], byte2=g_packet[1], byte3=g_packet[2];
	int LB,MB,RB,XOV,YOV;

	//Save byte1 bits meanings
	LB = byte1 & LB_BIT;
	RB = (byte1 & RB_BIT) >> 1;
	MB = (byte1 & MB_BIT) >> 2;
	XOV = (byte1 & XOV_BIT) >> 6;
	YOV = (byte1 & YOV_BIT) >> 7;


	//Print the packet as requested
	printf("B1=0x%02X B2=0x%02X B3=0x%02X LB=%d MB=%d RB=%d XOV=%d YOV=%d", byte1, byte2, byte3, LB, MB, RB, XOV, YOV);

	check_coordinates(&x,&y);

	printf(" X=%d",x);

	printf(" Y=%d\n",y);

}

int issue_kbc_cmd(unsigned char cmd)
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


int set_stream_mode(int mode)
{
	if(mode==0){

		//Enable data reporting
		issue_kbc_cmd (WR_MOUSE);
		sys_outb(IN_BUF, DATA_REPORT);

		if(read_ACK_byte()!=0)
			return -1;
	}

	//Set stream mode
	issue_kbc_cmd (WR_MOUSE);
	sys_outb(IN_BUF, STREAM_MODE);

	if(read_ACK_byte()!=0)
		return -1;

	return 0;

}

int disable_data_reporting(int mode)
{
	issue_kbc_cmd (WR_MOUSE);
	sys_outb(IN_BUF, DISABLE);

	if(read_ACK_byte()!=0)
		return -1;

	if(mode==0){
		//Set remote mode
		issue_kbc_cmd (WR_MOUSE);
		sys_outb(IN_BUF, RM_MODE);

		if(read_ACK_byte()!=0)
			return -1;
	}

	return 0;
}


void write_comm_byte(unsigned char comm)
{
	//Writes command byte
	sys_outb(IN_BUF,comm);
}

void read_comm_byte(unsigned char *comm)
{
	unsigned long comm1;

	//Reads command byte to comm1
	sys_inb(OUT_BUF,&comm1);

	//Stores value in the correct format
	*comm = (unsigned char) comm1;

}

void disable_IH()
{
	unsigned char comm;

	//Reads command byte
	issue_kbc_cmd(READ_COMM);
	read_comm_byte(&comm);

	//Writes command byte disabling IH
	issue_kbc_cmd(WRITE_COMM);
	write_comm_byte(comm & DISABLE_INT);
}

void enable_IH()
{
	unsigned char comm;

	//Reads command byte
	issue_kbc_cmd(READ_COMM);
	read_comm_byte(&comm);

	//Writes command byte enabling IH
	issue_kbc_cmd(WRITE_COMM);
	write_comm_byte(comm | INT_M_KBC | INT_KBC);
}


int read_ACK_byte()
{
	unsigned long ack_byte;
	unsigned long stat;

	while(1){
		if(sys_inb(STAT_REG, &stat)!=0)
			return -1;

		if(stat & OBF)
			sys_inb(OUT_BUF, &ack_byte);

		//Checks acknowledgment byte
		if(ack_byte==ACK)
			return 0;
		else if (ack_byte==ERROR)
			return -1;
		else
			continue;
	}
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


int write_to_mouse(unsigned char cmd)
{
	unsigned long stat;

	//Issue kbc_cmd that we'll write to the mouse
	issue_kbc_cmd (WR_MOUSE);

	//reads the status register to see if the input buffer is empty
	sys_inb(STAT_REG, &stat);

	//if empty, then write cmd to the input buffer, else try again
	if((stat & IBF) == 0){
		sys_outb(IN_BUF, cmd);
		//read acknowledgement byte
		read_ACK_byte();
		return 0;
	}

	return -1;
}



void check_event(ev_type_t evt, state_t *st)
{
	switch(*st) {
	case INIT:
		if(evt == RDOWN)
			*st = DRAW;
		break;
	case DRAW:
		if(evt == RUP || evt == OPPDIR)
			*st = INIT;
		else if(evt == MOVE)
			*st = COMP;
		break;
	default:
		break;
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

int downwards_pos_slope(short *length, short *total_length, int *x, int *y, state_t *st)
{

	//If the user goes in the opposite direction, reset
	if(*x >= 0 || *y >= 0)
	{
		check_event(OPPDIR,st);
		*total_length = 0;
		return -1;
	}

	//Sum the total length that the mouse has moved
	*total_length += (short) *x;

	//If the mouse has moved a greater or equal length to the one specified, go to COMP state
	if(*total_length <= *length)
		check_event(MOVE,st);

	return 0;
}

int upwards_pos_slope(short *length, short *total_length, int *x, int *y, state_t *st)
{
	//If the user goes in the opposite direction, reset
	if(*x <= 0 || *y <= 0)
	{
		check_event(OPPDIR,st);
		*total_length = 0;
		return -1;
	}

	//Sum the total length that the mouse has moved
	*total_length += (short) *x;

	//If the mouse has moved a greater or equal length to the one specified, go to COMP state
	if(*total_length >= *length)
		check_event(MOVE,st);

	return 0;
}

int check_movement(short *length, short *total_length, int *x, int *y, state_t *st)

{
	//If the line should be in upwards direction
	if(*length > 0){
		if(upwards_pos_slope(length, total_length, x, y,st)!=0)
			return -1;
	}
	//Otherwise if it should be in downwards direction
	else if(*length < 0)
	{
		if(downwards_pos_slope(length, total_length, x, y,st)!=0)
			return -1;
	}
	return 0;
}


int check_button(short *length, short *total_length, int *x, int *y,state_t *st)
{
	//Check if the right button is pressed -> RDOWN
	if(g_packet[0] & RB_BIT){
		check_event(RDOWN,st);

		//If RDOWN, then check mouse movement
		if(check_movement(length, total_length, x, y,st)!=0)
			return -1;
	}
	//Otherwise, back to initial state -> RUP
	else if((g_packet[0] & RB_BIT) == 0)
	{
		check_event(RUP,st);
		*total_length = 0;
		return -1;
	}

	return 0;
}

