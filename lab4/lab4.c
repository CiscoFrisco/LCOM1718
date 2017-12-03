#include <minix/drivers.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "test4.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);
static long parse_long(char *str, int base);

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
			"\t service run %s -args \"packet <decimal no. - number of packets>\"\n"
			"\t service run %s -args \"async <decimal no. - time>\"\n"
			"\t service run %s -args \"remote <decimal no. - period> <decimal no. - number of packets>\"\n"
			"\t service run %s -args \"gesture <decimal no. - minimum length>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned long number, time;
	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf("test4: wrong no. of arguments for mouse_test_packet()\n");
			return 1;
		}
		number = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (number == ULONG_MAX)
			return 1;
		printf("test4::mouse_test_packet(%lu)\n", number);
		return mouse_test_packet(number);
	}
	else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf("test4: wrong no. of arguments for mouse_test_async()\n");
			return 1;
		}
		time = parse_ulong(argv[2], 10);
		if (time == ULONG_MAX)
			return 1;
		printf("test4:mouse_test_async(%lu)\n",time);
		return mouse_test_async(time);
	}
	else if (strncmp(argv[1], "remote", strlen("remote")) == 0) {
		if (argc != 4) {
			printf("test4: wrong no of arguments for mouse_test_remote()\n");
			return 1;
		}
		/* Parses strings to unsigned longs */
		time = parse_ulong(argv[2], 10);
		number = parse_ulong(argv[3], 10);
		if (time == ULONG_MAX || number == ULONG_MAX)
			return 1;
		printf("test4::mouse_test_remote(%lu, %lu)\n", time, number);
		return mouse_test_remote(time, number);
	}
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
			if (argc != 3) {
				printf("test4: wrong no. of arguments for mouse_test_gesture()\n");
				return 1;
			}
			number = parse_long(argv[2], 10);/* Parses string to long */
			if (number == LONG_MAX || number == LONG_MIN)
				return 1;
			printf("test4::mouse_test_gesture(%ld)\n", number);
			return mouse_test_gesture(number);
		}



	else {
		printf("test4: %s - no valid function!\n", argv[1]);
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
		printf("test4: parse_ulong: no digits were found in %lu\n", str);
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
		printf("test4: parse_long: no digits were found in %l\n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}


