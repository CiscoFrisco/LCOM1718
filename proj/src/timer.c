#include <minix/driver.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include "timer.h"
#include "i8254.h"

//Global variables
int g_timer_hook_id = 1;		/**< @brief  Timer's Hook ID */
int g_counter = 0;				/**< @brief  Timer's Interrupt Counter */
static int hook_id_bit;			/**< @brief  Timer's Hook ID bitmask */

//Functions

int timer_subscribe_int()
{
	//Check range
	if(g_timer_hook_id > 31 || g_timer_hook_id < 0)
		return -1;

	hook_id_bit = BIT(g_timer_hook_id); //Save return value (bitmask) before changing hook_id

	//subscribe interrupt
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &g_timer_hook_id) != OK)
		return -1;

	return hook_id_bit;
}

int timer_unsubscribe_int()
{
	//unsubscribe interrupt
	if(sys_irqrmpolicy(&g_timer_hook_id) != OK)
		return -1;

	return 0;
}

void timer_int_handler()
{
	g_counter++;
}

int get_timer_hook_id()
{
	return hook_id_bit;
}
