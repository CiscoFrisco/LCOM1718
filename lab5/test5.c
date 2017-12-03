#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include "video_test.h"
#include "video_gr.h"
#include "timer.h"
#include "keyboard.h"
#include "i8042.h"
#include "vg_macros.h"
#include "vbe.h"
#include "sprite.h"

//Global variable that holds keyboard scancodes
unsigned char g_data;

int video_test_init(unsigned short mode, unsigned short delay) {

	//Stores bit mask in irq_timer and subscribes interrupts
	int irq_timer = timer_subscribe_int();

	int counter=0;//Counter counts the number of timer interrupts
	unsigned short time=0; //time measures the passage of time
	int ipc_status, r;
	message msg;

	//Initialize graphics to given mode
	if(vg_init(mode)==NULL){
		vg_exit();
		return -1;
	}


	/* Verifies whether minix has spent "delay" amount of time in graphics mode, if so the cicle should end
	 */
	while(time<delay) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
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

	//Exit to text mode
	if(vg_exit()!=0)
		return -1;

	printf("Back to Text mode!\n");

	return 0;
}


int video_test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {

	int ipc_status, r;
	message msg;

	//Initialize graphics mode
	if(vg_init(DEF_MODE)==NULL)
		return -1;

	//Draw a square on the screen
	if(vg_square(x,y,size,color)!=0)
		return -1;

	video_dump_fb();

	//Subscribe keyboard interrupts
	int irq_keyboard = kbd_subscribe_int();

	if(irq_keyboard == -1)
		return -1;

	//Verifies whether g_data holds the ESC break code, if so the cicle should end
	while(g_data != CVT_BREAK(ESC_KEY)) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_keyboard){
					//Read scancodes from the keyboard.
					kbd_c_handler();
				}
				break;
			default:
				break;
			}
		}
		else {}
	}

	//Exit graphics to text mode
	if(vg_exit()!=0)
		return -1;

	printf("\nESC breakcode received!\nExited Graphics mode\n");

	//Unsubscribes interrupts
	if(kbd_unsubscribe_int()!= 0)
		return -1;

	return 0;
}

int video_test_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color) {

	//Subscribe keyboard interrupts
	int irq_keyboard = kbd_subscribe_int();

	if(irq_keyboard == -1)
		return -1;

	int ipc_status, r;
	message msg;

	//Initialize graphics mode
	if(vg_init(DEF_MODE)==NULL)
		return -1;

	//Draw a line between the given coordinates
	if(vg_line(xi,yi,xf,yf,color)!=0)
		return -1;

	video_dump_fb();

	//Verifies whether g_data holds the ESC break code, if so the cicle should end
	while(g_data != CVT_BREAK(ESC_KEY)) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_keyboard){
					//Read scancodes from the keyboard.
					kbd_c_handler();
				}
				break;
			default:
				break;
			}
		}
		else {}
	}

	//Exit graphics to text mode
	if(vg_exit()!=0)
		return -1;

	printf("\nESC breakcode received!\nExited Graphics mode\n");

	//Unsubscribes interrupts
	if(kbd_unsubscribe_int()!= 0)
		return -1;

	return 0;

}

int video_test_xpm(char *xpm[], unsigned short xi, unsigned short yi) {

	int ipc_status, r;
	message msg;

	//Initialize graphics mode
	if(vg_init(DEF_MODE)==NULL)
		return -1;

	//Draws the xpm at the specified coordinates
	if(draw_xpm(xpm,xi,yi)!=0)
		return -1;

	video_dump_fb();

	//Subscribe keyboard interrupts
	int irq_keyboard = kbd_subscribe_int();

	if(irq_keyboard == -1)
		return -1;

	//Verifies whether g_data holds the ESC break code, if so the cicle should end
	while(g_data != CVT_BREAK(ESC_KEY)) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_keyboard){
					//Read scancodes from the keyboard.
					kbd_c_handler();
				}
				break;
			default:
				break;
			}
		}
		else {}
	}

	//Exit graphics to text mode
	if(vg_exit()!=0)
		return -1;

	printf("\nESC breakcode received!\nExited Graphics mode\n");

	//Unsubscribes interrupts
	if(kbd_unsubscribe_int()!= 0)
		return -1;

	return 0;
}

int video_test_move(char *xpm[], unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, short speed, unsigned short frame_rate) {


	int ipc_status, r;
	message msg;
	int counter=0; //counts the number of interrupts
	int frames = 60 / frame_rate; //no of interrupts until it's time for a new frame
	int frame_counter = 0;

	//Initialize graphics mode
	if(vg_init(DEF_MODE)==NULL)
		return -1;

	//Subscribe keyboard and timer interrupts
	int irq_keyboard = kbd_subscribe_int();
	int irq_timer = timer_subscribe_int();

	if(irq_keyboard == -1 || irq_timer ==-1)
		return -1;

	//Only accept a single axis movement
	if(xi!=xf && yi!=yf)
		return -1;

	Sprite* sp;

	//Determine in which axis the xpm "accelerates"
	if(xi==xf)
		sp = create_sprite(xpm, xi, yi,0,speed);
	else
		sp = create_sprite(xpm, xi, yi,speed,0);


	//Verifies whether g_data holds the ESC break code, if so the cicle should end
	while(g_data != CVT_BREAK(ESC_KEY)) {

		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_keyboard){
					//Read scancodes from the keyboard.
					kbd_c_handler();
				}

				if(msg.NOTIFY_ARG & irq_timer){
					counter++;

					//If speed is 0, the xpm doesn't move
					if(speed==0){
						draw_xpm(xpm,sp->x,sp->y);
						video_dump_fb();
						continue;
					}

					//Draw the xpm in a new position when the selected number of interrutps has been reached
					if(((counter % frames) == 0) && (sp->x < xf || sp->y < yf)){
						frame_counter--; //only useful when the speed is negative, to know how many frames we need to wait until the xpm moves
						clear_screen();
						draw_xpm(xpm,sp->x,sp->y);
						video_dump_fb();

						if(speed > 0)
						{
							//Vertical movement
							if(sp->xspeed == 0)
								sp->y+=sp->yspeed;
							//Horizontal movement
							else
								sp->x+=sp->xspeed;
						}
						else if(speed < 0)
						{
							//Increment a coordinate based on the given coordinates and speed, and reset frame_counter
							if(sp->xspeed == 0 &&frame_counter == speed)
							{
								sp->y += 1;
								frame_counter = 0;
							}

							else if(sp->yspeed == 0 &&frame_counter == speed)
							{
								sp->x += 1;
								frame_counter = 0;
							}
						}

					}
				}
				break;
			default:
				break;
			}
		}
		else {}
	}

	destroy_sprite(sp);

	//Exit graphics to text mode
	if(vg_exit()!=0)
		return -1;

	printf("\nESC breakcode received!\nExited Graphics mode\n");

	//Unsubscribes interrupts
	if(kbd_unsubscribe_int()!= 0)
		return -1;

	return 0;

}

int video_test_controller() {

	vbe_controller_info_t vbi_p;

	//Read VBE controller info
	if(vbe_get_controller_info(&vbi_p)!=0)
		return -1;

	//Stores the VRAM size in KBs
	unsigned int memory = vbi_p.TotalMemory*64;

	//Version in BCD
	unsigned short version = vbi_p.VbeVersion;
	//Separate major and minor version numbers
	unsigned minor = version & 0x00FF;
	unsigned major = (version & 0xFF00) >> 8;
	//Gets the address of the first byte of the Reserved area, which holds the video modes
	unsigned char *modes = vbi_p.Reserved;

	//Print information

	printf("%d.%d\n",major,minor);

	//While -1 hasn't been read, print the modes in the requested format
	while(*modes!=0xFF && *(modes+1)!=0xFF)
	{
		printf("0x%01X",*(modes+1));
		printf("%02X",*modes);
		printf(":");
		modes = modes + 2;

	}

	printf("\n%d\n",memory);

	return 0;

}

