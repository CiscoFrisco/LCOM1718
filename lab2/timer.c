#include <minix/syslib.h>
#include <minix/drivers.h>
#include "timer.h"
#include "i8254.h"

//Global variables
int g_hook_id = 1;
int g_counter = 0;

//Functions

int timer_set_frequency(unsigned char timer, unsigned long freq) {

	unsigned char config; //stores timer configuration
	unsigned long mask = 0x0F; //00001111
	unsigned int div; //value to write to the counter, according with given freq
	unsigned char div_MSB, div_LSB; //in order to write the the full value, separate in LSB and MSB

	timer_get_conf(timer,&config); //reads timer configuration to config

	config = config & mask; //makes sure 4 most significant bits are set to 0 to avoid problems

	//put desired bits to 1 (timer and read/write)
	switch(timer)
	{
	case 0:
		config = (config | TIMER_SEL0 | TIMER_LSB_MSB);
		break;
	case 1:
		config = (config | TIMER_SEL1 | TIMER_LSB_MSB);
		break;
	case 2:
		config = (config | TIMER_SEL2 | TIMER_LSB_MSB);
		break;
	default:
		return 1;
	}


	sys_outb(TIMER_CTRL, config); // writes control word informing we will write to the counter

	div =  TIMER_FREQ/freq;
	div_MSB = div >> 8; //8 most significant bits
	div_LSB = (div << 8) >> 8; //8 less significant bits

	//Write value to the counter (LSB then MSB)
	switch(timer)
	{
	case 0:
		sys_outb(TIMER_0, div_LSB);
		sys_outb(TIMER_0, div_MSB);
		break;
	case 1:
		sys_outb(TIMER_1, div_LSB);
		sys_outb(TIMER_1, div_MSB);
		break;
	case 2:
		sys_outb(TIMER_2, div_LSB);
		sys_outb(TIMER_2, div_MSB);
		break;
	}

	return 0;
}

int timer_test_time_base(unsigned long freq) {

	//Sets TIMER_0 frequency
	if(timer_set_frequency(0,freq)==0)
		return 0;

	return 1;
}

int timer_subscribe_int(void ) {
	//Check range
	if(g_hook_id > 31 || g_hook_id < 0)
		return -1;

	int hook_id_bit = BIT(g_hook_id); //Save return value (bitmask)before changing hook_id

	//subscribe interrupt
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &g_hook_id) != OK)
		return -1;

	return hook_id_bit;
}

int timer_unsubscribe_int() {

	//unsubscribe interrupt
	if(sys_irqrmpolicy(&g_hook_id) != OK)
		return -1;

	return 0;
}

void timer_int_handler() {
	g_counter++;
}

int timer_test_int(unsigned long time) {
	//Time cannot be negative
	if(time <= 0)
		return 1;

	//Stores bit mask in irq_set and subscribes interrupts
	int irq_set = timer_subscribe_int();

	int ipc_status, r, freq = 60, c=1;
	message msg;

	 while(g_counter < time*freq) {
		 if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			 printf("driver_receive failed with: %d", r);
			 continue;
		 }
		 if (is_ipc_notify(ipc_status)) {
			 switch (_ENDPOINT_P(msg.m_source)) {
			 case HARDWARE:
				 if (msg.NOTIFY_ARG & irq_set){ //Calls interrupt handler and prints message
					 timer_int_handler();
					 if(g_counter % freq == 0){
						 printf("Message %d/%d\n",c,time);
						 c++;
					 }

				 }
				 break;
			 default:
				 break;
			 }
		 }
		 else {}
	 }

	timer_unsubscribe_int(); //Unsubscribes interrupts

	return 0;
}

int timer_get_conf(unsigned char timer, unsigned char *st) {
	//Check range
	if(timer > 2)
		return 1;
	else
	{
		unsigned long Read_Back;
		unsigned long st1; //stores config

		//write Read_Back command and read the configuration
		if(timer == 0)
		{
			Read_Back = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(0);

			sys_outb(TIMER_CTRL,Read_Back);
			sys_inb(TIMER_0,&st1);
		}

		else if(timer == 1)
		{
			Read_Back = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(1);

			sys_outb(TIMER_CTRL,Read_Back);
			sys_inb(TIMER_1,&st1);
		}

		else
		{
			Read_Back = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(2);

			sys_outb(TIMER_CTRL,Read_Back);
			sys_inb(TIMER_2,&st1);
		}

		*st= (unsigned char) st1; //adjusting format
	}
	return 0;
}

int timer_display_conf(unsigned char conf)
{
	unsigned int BCD = conf & TIMER_BCD; //Stores BCD bit value
	unsigned int Mode = (conf & TIMER_MODES) >> 1; //Stores Mode bits value
	unsigned int Read_Write = (conf & TIMER_LSB_MSB) >> 4; //Stores R/W bits value
	unsigned int Null_Count = (conf & TIMER_NULLCOUNT) >> 6; //Stores Null_Count bit value
	unsigned int Output = (conf & TIMER_OUTPUT) >> 7; //Stores Output bit value

	printf("\nDisplaying timer configuration\n\n");

	//Prints BCD's bit meaning
	if(BCD == 0)
		printf("Binary Counter\n");
	else
		printf("Binary Coded Decimal\n");

	//Prints Mode bits meaning
	if(Mode == 6 || Mode==7)
		printf("Mode %d\n",Mode-4);
	else
		printf("Mode %d\n",Mode);

	//Prints R/W bits meaning
	switch(Read_Write)
	{
	case 0:
		printf("Counter Latch Command\n");
	case 1:
		printf("Read/Write LSB\n");
	case 2:
		printf("Read/Write MSB\n");
	case 3:
		printf("Read/Write LSB first and then MSB\n");
	}

	//Prints Null_Count bit meaning
	if(Null_Count == 0)
		printf("Count available for reading\n");
	else
		printf("Null Count\n");

	//Prints Output bit meaning
	printf("Output pin is %d\n",Output);


	return 0;
}

int timer_test_config(unsigned char timer) {
	
	unsigned char address; //To store timer's configuration
	int success=1; //Checking weather functions are working properly

	//Gets timer's configuration
	if(timer_get_conf(timer, &address)==0)
		success=0;

	//Displays timer's configuration in human friendly way
	if(timer_display_conf(address)==1)
		success=1;

	return success;
}
