#include <minix/driver.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <stdio.h>
#include "rtc.h"
#include "rtc_macros.h"


/* GLOBAL VARIABLES */

extern unsigned char reg_A;
extern unsigned char reg_B;
extern unsigned char g_rtc_seconds;
extern unsigned char g_rtc_minutes;
extern unsigned char g_rtc_hours;
extern unsigned char g_rtc_day;
extern unsigned char g_rtc_month;
extern unsigned char g_rtc_year;
int g_rtc_hook_id = 8;				/**< @brief  RTC's Hook ID */
static int hook_id_bit;				/**< @brief  RTC's Hook ID bitmask*/

/* FUNCTIONS */

int get_rtc_hook_id()
{
	return hook_id_bit;
}

unsigned int interpret_BCD(unsigned char value)
{
	return ((value & 0xF0) >> 4) * 10 + (value & 0x0F);
}

Time read_time(){

	wait_valid_rtc();

	//Read RTC's time registers
	rtc_assembly_handler();

	//Store the already converted values in convenient variables
	unsigned int sec = interpret_BCD(g_rtc_seconds);
	unsigned int min = interpret_BCD(g_rtc_minutes);
	unsigned int hour = interpret_BCD(g_rtc_hours);
	unsigned int day = interpret_BCD(g_rtc_day);
	unsigned int month = interpret_BCD(g_rtc_month);
	unsigned int year = interpret_BCD(g_rtc_year);

	//Create a time object
	Time time = create_time(sec, min, hour, day, month, year);

	return time;
}

void program_reg_B()
{
	read_reg_B();

	//Program RTC to use 24h and BCD values on time registers
	reg_B |= set24;
	reg_B &= setBCD;

	write_reg_B();
}

void read_reg_B()
{
	unsigned long reg_B1=0;

	//Read RTC's Register B and store it in a convenient variable
	sys_outb(RTC_ADDR_REG, RTC_REG_B);
	sys_inb(RTC_DATA_REG, &reg_B1);
	reg_B = (unsigned char) reg_B1;
}

void write_reg_B()
{
	sys_outb(RTC_ADDR_REG, RTC_REG_B);
	sys_outb(RTC_DATA_REG, reg_B);
}

void disable_interrupts()
{
	read_reg_B();

	//Disable interrupts
	reg_B &= disINT;

	write_reg_B();

}

void enable_interrupts(){

	read_reg_B();

	//Enable interrupts
	reg_B |= enINT;

	write_reg_B();

}

void wait_valid_rtc()
{
	unsigned long reg_A1 = 0;

	do
	{
		disable_interrupts();
		sys_outb(RTC_ADDR_REG, RTC_REG_A);
		sys_inb(RTC_DATA_REG, &reg_A1);
		enable_interrupts();
	} while (reg_A1 & RTC_UIP);

	reg_A = (unsigned char) reg_A1;
}

int rtc_subscribe_int()
{
	//Check hook_id range
	if(g_rtc_hook_id > 31 || g_rtc_hook_id < 0)
		return -1;

	//Save return value (bitmask) before changing hook_id
	hook_id_bit = BIT(g_rtc_hook_id);

	//Subscribe interrupt
	if(sys_irqsetpolicy(RTC_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE),
			&g_rtc_hook_id) != OK)
		return -1;

	return hook_id_bit;
}

int rtc_unsubscribe_int()
{
	//Unsubscribe interrupt
	if(sys_irqrmpolicy(&g_rtc_hook_id) != OK)
		return -1;

	return 0;
}

