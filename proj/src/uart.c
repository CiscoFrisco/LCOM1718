#include <minix/driver.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <stdio.h>
#include "uart.h"
#include "uart_macros.h"
#include "keyboard.h"

int g_uart_hook_id = 7;  						/**< @brief  UART's Hook ID */
static int hook_id_bit; 						/**< @brief  UART's Hook ID Bitmask */
extern unsigned char g_uart_rbr_data; 			/**< @brief  UART's Receiver Data */
extern unsigned char g_kbd_data;

int subscribe_uart()
{
	if(g_uart_hook_id > 31 || g_uart_hook_id < 0)
		return -1;

	//Save return value (bitmask) before changing hook_id
	hook_id_bit = BIT(g_uart_hook_id);

	//Subscribe interrupt
	if(sys_irqsetpolicy(UART_IRQ_COM1, (IRQ_REENABLE | IRQ_EXCLUSIVE),
			&g_uart_hook_id) != OK )
		return -1;

	return hook_id_bit;
}

int unsubscribe_uart()
{
	if(sys_irqrmpolicy(&g_uart_hook_id) != OK)
		return -1;

	return 0;
}

int get_uart_hook_id()
{
	return hook_id_bit;
}

void setup_LCR()
{
	unsigned long lcr;
	unsigned char lcr_ch;

	//Read LCR register and store it in a convenient variable
	sys_inb(LCR, &lcr);
	lcr_ch = (unsigned char) lcr;

	//Set 8 bits per char, 2 stop bits, even parity, and DLA access
	lcr_ch |= LCR_CONF1;
	lcr_ch &= LCR_CONF2;
	sys_outb(LCR, lcr_ch);

	//Set bit rate
	sys_outb(BASE_ADD, DIVISOR_LSB);
	sys_outb(IER, DIVISOR_MSB);

	//Write to the LCR
	sys_outb(LCR, lcr_ch & LCR_CONF3);
}

void setup_IER()
{
	unsigned long ier;
	unsigned char ier_ch;

	//Read IER register and store it in a convenient variable
	sys_inb(IER, &ier);
	ier_ch = (unsigned char) ier;

	//Enable all interrupts, except MODEM
	ier_ch |= IER_CONF;
	sys_outb(IER, ier_ch);
}

int check_RBR()
{
	unsigned long lsr;

	sys_inb(LSR, &lsr);

	//If the Receiver Buffer Register has data available for reading
	if(lsr & RD)
		return 0;

	return 1;
}

int check_THEmpty()
{
	unsigned long lsr;

	sys_inb(LSR, &lsr);

	//If the Transmitter Holding Register is empty
	if(lsr & THRE)
		return 0;

	return 1;
}

void send_data(unsigned char data)
{
	//If the Transmitter Holding Register is empty, then send data

	while(check_THEmpty() != 0){}

	sys_outb(BASE_ADD, data);
}

void clearRBR()
{
	unsigned long temp_rbr = 0;

	//If there's data to read, store it in a global variable
	if(check_RBR() == 0)
	{
		sys_inb(BASE_ADD, &temp_rbr);
		g_uart_rbr_data = (unsigned char) temp_rbr;
	}
}

void uart_handler(){

	unsigned long iir = 0;
	unsigned long temp_rbr = 0;

	//Read Interrupt Identification Register
	sys_inb(IIR, &iir);

	iir = ((iir << 4) >> 4);

	if(iir == RDA_INT_IIR)
	{
		//If there's data to read, store it in a global variable
		if(check_RBR() == 0)
		{
			sys_inb(BASE_ADD, &temp_rbr);
			g_uart_rbr_data = (unsigned char) temp_rbr;
		}
	}

	else if(iir == RLS_INT_IIR)
	{
		sys_inb(LSR, &temp_rbr);
	}
}

void clear_uart_data()
{
	g_uart_rbr_data=0;
}
