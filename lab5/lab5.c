#include <minix/drivers.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "test5.h"
#include "video_test.h"
#include "pixmap.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"init <hexadecimal no. - mode> <decimal no. - delay>\"\n"
			"\t service run %s -args \"square <decimal no. - horizontal coordinate> <decimal no. - vertical coordinate> <decimal no. - size of square side in pixels> <hexadecimal no. - color>\"\n"
			"\t service run %s -args \"line <decimal no. - horizontal coordinate of first endpoint> <decimal no. - vertical coordinate of first endpoint> <decimal no. - horizontal coordinate of last endpoint> <decimal no. - vertical coordinate of last endpoint> <hexadecimal no. - color>\"\n"
			"\t service run %s -args \"xpm <char array - xpm> <decimal no. - x coordinate of upper left corner> <decimal no. - y coordinate of upper left corner>\"\n"
			"\t service run %s -args \"move <char array - xpm> <decimal no. - initial x coordinate of upper left corner> <decimal no. - initial y coordinate of upper left corner> <decimal no. - final x coordinate of upper left corner> <decimal no. - final y coordinate of upper left corner> <decimal no. - speed> <decimal no. framerate>\"\n"
			"\t service run %s -args \"controller\"\n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned long mode, delay, size, xi, yi, xf, yf, color, framerate;
	long speed;
	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf("test5: wrong no. of arguments for video_test_init()\n");
			return 1;
		}
		mode = parse_ulong(argv[2], 16);
		delay = parse_ulong(argv[3], 10);/* Parses string to unsigned long */
		if (mode == ULONG_MAX || delay == ULONG_MAX)
			return 1;
		printf("test5::video_test_init(0x%02X, %lu)\n", mode, delay);
		return video_test_init(mode, delay);
	}
	else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf("test5: wrong no. of arguments for video_test_square()\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);
		size = parse_ulong(argv[4], 10);
		color = parse_ulong(argv[5], 16);
		if (xi == ULONG_MAX || yi == ULONG_MAX || size == ULONG_MAX || color == ULONG_MAX)
			return 1;
		printf("test5:video_test_square(%lu, %lu, %lu, 0x%02X)\n",xi, yi, size, color);
		return video_test_square(xi, yi, size, color);
	}
	else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf("test5: wrong no of arguments for video_test_line()\n");
			return 1;
		}
		/* Parses strings to unsigned longs */
		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);
		xf = parse_ulong(argv[4], 10);
		yf = parse_ulong(argv[5], 10);
		color = parse_ulong(argv[6], 16);

		if (xi == ULONG_MAX || yi == ULONG_MAX || xf == ULONG_MAX || yf == ULONG_MAX || color == ULONG_MAX)
			return 1;
		printf("test5::video_test_line(%lu, %lu, %lu, %lu, 0x%02X)\n", xi,yi, xf, yf, color);
		return video_test_line(xi, yi, xf, yf, color);
	}
	else if(strncmp(argv[1], "xpm", strlen("xpm")) == 0){

		if (argc != 5) {
			printf("test5: wrong no of arguments for video_test_xpm()\n");
			return 1;
		}
		/* Parses strings to unsigned longs*/
		xi = parse_ulong(argv[3], 10);
		yi = parse_ulong(argv[4], 10);

		if (xi == ULONG_MAX || yi == ULONG_MAX)
			return 1;

		if(strncmp(argv[2], "pic1", strlen("pic1")) == 0){
			printf("test5::video_test_xpm(pic1, %lu, %lu)\n",xi,yi);
			return video_test_xpm(pic1,xi, yi);
		}
		else if(strncmp(argv[2], "pic2", strlen("pic2")) == 0){
			printf("test5::video_test_xpm(pic2, %lu, %lu)\n",xi,yi);
			return video_test_xpm(pic2,xi, yi);
		}
		else if(strncmp(argv[2], "pic3", strlen("pic3")) == 0){
			printf("test5::video_test_xpm(pic3, %lu, %lu)\n",xi,yi);
			return video_test_xpm(pic3,xi, yi);
		}
		else if(strncmp(argv[2], "cross", strlen("cross")) == 0){
			printf("test5::video_test_xpm(cross, %lu, %lu)\n",xi,yi);
			return video_test_xpm(cross,xi, yi);
		}
		else if(strncmp(argv[2], "penguin", strlen("penguin")) == 0){
			printf("test5::video_test_xpm(penguin, %lu, %lu)\n",xi,yi);
			return video_test_xpm(penguin,xi, yi);
		}
		else{
			printf("test5::video_test_xpm: no such pic available!");
			return 1;
		}
	}
	else if(strncmp(argv[1], "move", strlen("move")) == 0){
		if (argc != 9) {
			printf("test5: wrong no of arguments for video_test_move()\n");
			return 1;
		}
		/* Parses strings to unsigned longs*/
		xi = parse_ulong(argv[3], 10);
		yi = parse_ulong(argv[4], 10);
		xf = parse_ulong(argv[5], 10);
		yf = parse_ulong(argv[6], 10);
		speed = parse_long(argv[7], 10);
		framerate = parse_ulong(argv[8], 10);

		if (xi == ULONG_MAX || yi == ULONG_MAX || xf == ULONG_MAX || yf == ULONG_MAX || framerate == ULONG_MAX || speed == LONG_MAX || speed == LONG_MIN)
			return 1;

		if(strncmp(argv[2], "pic1", strlen("pic1")) == 0){
			printf("test5::video_test_move(pic1, %lu, %lu, %lu, %lu, %d, %lu)\n",xi,yi,xf,yf,speed,framerate);
			return video_test_move(pic1,xi, yi,xf,yf,speed,framerate);
		}
		else if(strncmp(argv[2], "pic2", strlen("pic2")) == 0){
			printf("test5::video_test_move(pic2, %lu, %lu, %lu, %lu, %d, %lu)\n",xi,yi,xf,yf,speed,framerate);
			return video_test_move(pic2,xi, yi,xf,yf,speed,framerate);
		}
		else if(strncmp(argv[2], "pic3", strlen("pic3")) == 0){
			printf("test5::video_test_move(pic3, %lu, %lu, %lu, %lu, %d, %lu)\n",xi,yi,xf,yf,speed,framerate);
			return video_test_move(pic3,xi, yi,xf,yf,speed,framerate);
		}
		else if(strncmp(argv[2], "cross", strlen("cross")) == 0){
			printf("test5::video_test_move(cross, %lu, %lu, %lu, %lu, %d, %lu)\n",xi,yi,xf,yf,speed,framerate);
			return video_test_move(cross,xi, yi,xf,yf,speed,framerate);
		}
		else if(strncmp(argv[2], "penguin", strlen("penguin")) == 0){
			printf("test5::video_test_move(penguin, %lu, %lu, %lu, %lu, %d, %lu)\n",xi,yi,xf,yf,speed,framerate);
			return video_test_move(penguin,xi, yi,xf,yf,speed,framerate);
		}
		else{
			printf("test5::video_test_move: no such pic available!");
			return 1;
		}
	}
	else if(strncmp(argv[1], "controller", strlen("controller")) == 0){
		if (argc != 2) {
			printf("test5: wrong no. of arguments for video_test_controller()\n");
			return 1;
		}
		printf("test5:video_test_controller()\n");
		return video_test_controller();
	}
	else {
		printf("test5: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("test5: parse_ulong: no digits were found in %lu\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static long parse_long(char *str, int base)
{
	char *endptr;
	long val;

	/* Convert string to unsigned long */
	val = strtol(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == LONG_MAX) ||  (errno != 0 && val == 0)) {
		perror("strtol");
		return LONG_MAX;
	}

	if((errno == ERANGE && val == LONG_MIN) ||  (errno != 0 && val == 0)){
		perror("strtol");
		return LONG_MIN;
	}

	if (endptr == str) {
		printf("test5: parse_long: no digits were found in %l\n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}
