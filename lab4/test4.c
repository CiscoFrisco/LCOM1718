#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include "test4.h"
#include "mouse.h"
#include "timer.h"
#include "i8042.h"


int mouse_test_packet(unsigned short cnt){

	//Stores bit mask in irq_set and subscribes interrupts
	int irq_set = mouse_subscribe_int();

	if(irq_set==-1)
		return -1;

	//Sets stream mode with data reporting
	if(set_stream_mode(0)!=0)
		return -1;


	int ipc_status, r, byte_cnt=0;
	unsigned short print_cnt=0;
	message msg;

	//Verifies whether print_cnt has reached cnt, if so the cicle should end
	while(print_cnt < cnt) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set){
					if(mouse_wrapper(&byte_cnt)==0){ //When a full packet has been read, print it
						print_packet();
						print_cnt++;
					}
				}
				else

					break;
			default:
				break;
			}
		}
		else {}
	}

	printf("\nProgram has successfully read %d packets!\n",cnt);


	//Disables data reporting
	if(disable_data_reporting(1)!=0)
		return -1;

	//Unsubscribe mouse interrupts
	if(mouse_unsubscribe_int()!=0)
		return -1;

	return 0;
}	

int mouse_test_async(unsigned short idle_time){

	//Stores bit mask in irq_kbd and irq_timer and subscribes interrupts for both devices
	int irq_mouse= mouse_subscribe_int();
	int irq_timer= timer_subscribe_int();

	if(irq_mouse==-1 || irq_timer==-1)
		return -1;

	//Sets stream mode with data reporting enabled
	if(set_stream_mode(0)!=0)
		return -1;

	int counter=0,byte_cnt=0; //Counter counts the number of timer interrupts
	unsigned short time=0; //time measures the passage of time
	int ipc_status, r;
	message msg;

	/* Verifies whether minix hasn't received any packets for idle_time seconds, if so the cicle should end
	 */
	while(time<idle_time) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_mouse){ //Calls interrupt handler and prints packets
					if(mouse_wrapper(&byte_cnt)==0){
						print_packet();
						time=0; //Resets time, because a packet was received
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


	printf("\nProgram has been idle for %d seconds!\n",idle_time);

	//Disables data reporting
	if(disable_data_reporting(1)!=0)
		return -1;

	//Unsubscribes interrupts
	if(mouse_unsubscribe_int()!=0)
		return -1;

	if(timer_unsubscribe_int()!=0)
		return -1;

	return 0;

}


int mouse_test_remote(unsigned long period, unsigned short cnt){

	unsigned short print_cnt = 0;
	int byte_cnt=0;

	//subscribe keyboard in exclusive mode so that "7" doesn't appear on the screen
	if(kbd_subscribe_int() == -1)
		return -1;

	//Disable minix's mouse interrupt handler
	disable_IH();

	//Set remote mode
	if(disable_data_reporting(0)!=0)
		return -1;

	//Loop ends when print_cnt(variable that increments on every printed packet) is equal to the function argument.
	while(print_cnt < cnt)
	{
		if(write_to_mouse(READ_DATA)!=0)
			continue;

		//read a whole packet, then print it
		while(mouse_wrapper(&byte_cnt)!=0){}
		print_packet();
		print_cnt++;

		//wait before next iteration to display periodically
		tickdelay(micros_to_ticks(period*1000));
	}

	printf("\nProgram has successfully displayed %d packets every %d miliseconds!\n", cnt, period);

	//Set stream mode without data reporting
	if(set_stream_mode(1)!=0)
		return -1;

	//reenable minix's mouse interrupt handler
	enable_IH();

	//Unsubscribe keyboard interrupts
	if(kbd_unsubscribe_int() != 0)
		return -1;

	return 0;
}

int mouse_test_gesture(short length){

	//Subscribe mouse interrupts
	int irq_set = mouse_subscribe_int();

	//If there was an error in the mouse's interrupts subscription
	if(irq_set==-1)
		return -1;

	//total length of the gesture (in the x axis)
	short total_length = 0;

	//decimal values of our mouse relative coordinates
	int x, y;

	int ipc_status, r, byte_cnt=0;
	message msg;
	static state_t st = INIT; // initial state; keep state

	//if there was an error setting stream_mode return -1
	if(set_stream_mode(0)!=0)
		return -1;

	//loop ends when the desired gesture is drawn and we jump to our accept state(COMP) in the state machine
	while(st!=COMP) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set){
					if(mouse_wrapper(&byte_cnt)==0){ //when a full packet has been read

						print_packet(); //print the packet

						check_coordinates(&x,&y); //Check x and y and update their values if they're supposed to be negative

						//Checks the state of our state machine and updates variables has necessary.
						check_button(&length, &total_length, &x,&y,&st);
					}
				}
				else
					break;
			default:
				break;
			}
		}
		else {}
	}

	printf("\nYou have drawn a line with positive slope and length in the x axis >= to %d!\n",abs(length));


	//Disables data reporting
	if(disable_data_reporting(1)!=0)
		return -1;

	//Unsubscribe mouse interrupts
	if(mouse_unsubscribe_int()!=0)
		return -1;

	return 0;

}
