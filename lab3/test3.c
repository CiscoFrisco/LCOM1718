#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include "keyboard.h"
#include "timer.h"
#include "i8042.h"

int g_call_cnt=0;
unsigned char g_data;

int kbd_test_scan(unsigned short ass) {

	//Stores bit mask in irq_set and subscribes interrupts
	int irq_set = kbd_subscribe_int();

	int ipc_status, r;
	message msg;
	unsigned int type;

	//Verifies whether g_data holds the ESC break code, if so the cicle should end
	while(g_data != CVT_BREAK(ESC_KEY)) {
			 if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
				 printf("driver_receive failed with: %d", r);
				 continue;
			 }

			 if (is_ipc_notify(ipc_status)) {
				 switch (_ENDPOINT_P(msg.m_source)) {
				 case HARDWARE:
					 if (msg.NOTIFY_ARG & irq_set){
						 //Read and print scan codes using handler written in C
						 if(ass==0){
							 if(kbd_c_wrapper(&type)!=-1)
								 print_code(&type);
						 }

						 //Read and print scan codes using handler written in assembly
						 else {
							 if(kbd_ass_wrapper(&type)!=-1)
								 print_code(&type);
						 }
					 }
					 break;
				 default:
					 break;
				 }
			 }
			 else {}
		 }

	printf("\nESC breakcode received!\n");

	//Prints the number of sys_inb calls if it's using C handler
	if(ass == 0)
		printf("\nNumber of sys_inb() calls: %d\n", g_call_cnt);

	//Unsubscribes interrupts
	kbd_unsubscribe_int();

	return 0;
}

int kbd_test_poll() {

	unsigned int type;

	//Verifies whether g_data holds the ESC break code, if so the cicle should end
	while(g_data != CVT_BREAK(ESC_KEY)){

		//Reads scancodes and prints them
		if(kbd_c_wrapper(&type)!= -1)
			print_code(&type);

	}

	printf("\nESC breakcode received!\n");


	//Prints the number of sys_inb calls
	printf("\nNumber of sys_inb() calls: %d\n", g_call_cnt);

	//Enables keyboard interrupts
	enable_interrupt();

	return 0;
}

int kbd_test_timed_scan(unsigned short n) {

	//Stores bit mask in irq_kbd and irq_timer and subscribes interrupts for both devices
	int irq_kbd= kbd_subscribe_int();
	int irq_timer= timer_subscribe_int();


	int counter=0, time=0; //Counter counts the number of timer interrupts, time measures the passage of time
	int ipc_status, r;
	message msg;
	unsigned int type;

	/* Verifies whether g_data holds the ESC break code or if minix hasn't received any
	 * scancodes for n seconds, if so the cicle should end
	 */
	while(g_data != CVT_BREAK(ESC_KEY) && time<n) {
			 if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
				 printf("driver_receive failed with: %d", r);
				 continue;
			 }

			 if (is_ipc_notify(ipc_status)) {
				 switch (_ENDPOINT_P(msg.m_source)) {
				 case HARDWARE:
					 if (msg.NOTIFY_ARG & irq_kbd){ //Calls interrupt handler and prints scancodes
						 if(kbd_c_wrapper(&type)!=-1){
							 print_code(&type);
							 time=0; //Resets time, because a scancode was received
						 }
					 }
					 if(msg.NOTIFY_ARG & irq_timer){
						 counter++;
						 if(counter % 60 ==0)
							 time++; //Increments time every second
					 }
					 break;
				 default:
					 break;
				 }
			 }
			 else {}
		 }

	if(g_data == CVT_BREAK(ESC_KEY))
		printf("\nESC breakcode received!\n");
	else
		printf("\nProgram has been idle for %d seconds!\n",n);


	//Unsubscribes interrupts
	kbd_unsubscribe_int();
	timer_unsubscribe_int();

	return 0;
}
