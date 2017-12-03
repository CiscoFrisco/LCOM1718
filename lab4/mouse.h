#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

//state machine states and events
typedef enum {INIT, DRAW, COMP} state_t;
typedef enum {RUP, RDOWN, MOVE, OPPDIR} ev_type_t; //OPPDIR -> Opposite direction

/*
 * Subscribes mouse interrupts, similar function to the one from lab3, and prevents
 * standard IH from being notified of the occurrence of KBC interrupts.
 */
int mouse_subscribe_int();

/*
 * Unsubscribes keyboard interrupts, similar function to the one from lab2
 */
int mouse_unsubscribe_int();

/*
 * Reads whatever data is in OUT_BUF and stores it in g_data. If it detects and error,
 * then puts g_handler_err to -1.
 */
void mouse_handler();

/*
 * Prints the packet stored in g_packet[].
 */
void print_packet();

/*
 * Sets stream mode. If mode==0, also enables data reporting.
 */
int set_stream_mode(int mode);

/*
 * Issues a command to the KBC if IBF is not set.
 */
int issue_kbc_cmd(unsigned char cmd);

/*
 * Calls mouse_handler() and stores g_data in g_packet. Then, if appropriate
 * (when the handler reads 3 bytes) , calls print_packet().
 * Returns 0 after reading a whole packet, 1 if it didn't read the entire packet, and -1
 * in case of an error.
 */
int mouse_wrapper(int *cnt);

/*
 * Disables data reporting. If mode==0, also sets remote mode.
 */
int disable_data_reporting(int mode);
/*
 * Disables minix's mouse interrupt handler so that it doesn't steals mouse packets,
 * by writing to the KBC's command byte.
 */
void disable_IH();

/*
 * Enables minix's mouse interrupt handler.
 */
void enable_IH();

/*
 * Reads acknowledgement byte. In case it reads ACK, returns 0. If ERROR, returns -1. If NACK,
 * tries again.
 */
int read_ACK_byte();

/*
 * Reads command byte.
 */
void read_comm_byte(unsigned char *comm);

/*
 * Subscribes keyboard interrupts in exclusive mode.
 */
int kbd_subscribe_int();

/*
 * Unsubscribes keyboard interrupts
 */
int kbd_unsubscribe_int();

/*
 *Writes a command to the mouse
 */
int write_to_mouse(unsigned char cmd);

/*
 *Calls downwards_pos_slope() or upwards_pos slope() depending on the length signal.
 */
int check_movement(short *length, short *total_length, int *x, int *y, state_t *st);

/*
 *Verifies whether right hand button was pressed and calls check_movement(), otherwise resets state and length.
 */
int check_button(short *length, short *total_length, int *x, int *y, state_t *st);

/*
 * *Handles a positive slope line going downwards. Prevents change of direction.
 */
int downwards_pos_slope(short *length, short *total_length, int *x, int *y, state_t *st);

/*
 *Handles a positive slope line going upwards.Prevents change of direction.
 */
int upwards_pos_slope(short *length, short *total_length, int *x, int *y, state_t *st);

/*
 * Checks the signal of the coordinates and updates it accordingly.
 */
void check_coordinates(int *x, int *y);




#endif
