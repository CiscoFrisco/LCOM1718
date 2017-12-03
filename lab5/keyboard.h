#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

extern int g_call_cnt;
extern unsigned char g_data;

/*
 * Wrapper function to count the number of sys_inb calls. Will not be used in the project.
 */
int sys_inb_cnt(port_t port, unsigned long *byte);

/*
 * Subscribes keyboard interrupts, similar function to the one from lab2, but prevents
 * standard IH from being notified of the occurrence of KBC interrupts.
 */
int kbd_subscribe_int();

/*
 * Unsubscribes keyboard interrupts, similar function to the one from lab2
 */
int kbd_unsubscribe_int();

/*
 * Reads whatever data is in OUT_BUF by calling kbd_c_handler() and stores it in g_data.
 * *type signals whether the function is reading a one (0) or two (1) byte scancode.
 * Useful for both kbd_test_scan and kbd_test_poll
 */
int kbd_c_wrapper(unsigned int *type);

/*
 * Same as previous function, except some of its code is written in assembly.
 */
int kbd_ass_wrapper(unsigned int *type);


/*
 * Prints the scancode stored in g_data. Just like the previous function, *type signals
 * whether the function is reading a one (0) or two (1) byte scancode.
 * Also makes the distinction between make or break codes.
 */
int print_code(unsigned int *type);

/*
 * Reads whatever data is in OUT_BUF and stores it in g_data.
 */
void kbd_c_handler();

/*
 * Reads whatever data is in OUT_BUF and stores it in g_data.
 */
unsigned long assemblyHandler();

/*
 * Issues a command byte if IBF is not set.
 */
int issue_comm_byte(unsigned char cmd);

/*
 * Reads the command byte and stores it in *comm
 */
void read_comm_byte(unsigned char *comm);

/*
 * Writes a command byte.
 */
void write_comm_byte(unsigned char comm);

/*
 * Enables interrupts on the keyboard. Necessary to continue using Minix's console
 * after calling kbd_test_poll
 */
int enable_interrupt();

/* If we're using this function inside lab3,
 * sys_inb_cnt() will implement the piece of code present in keyboard.c
 * else sys_inb_cnt() will replace sys_inb()
 */
#ifdef LAB3
int sys_inb_cnt(port_t port, unsigned long *byte);
#else
#define sys_inb_cnt(p,q) sys_inb(p,q)
#endif



#endif /* _LCOM_KEYBOARD_H */
