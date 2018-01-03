#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

/** @defgroup mouse mouse
 * @{
 *
 * Functions related to the mouse module
 */

/**
 * @brief Subscribes mouse interrupts
 *
 * Subscribes interrupts, preventing the standard IH from being notified of the occurrence of KBC interrupts.
 *
 * @return 0 is successful, 1 if not
 */
int mouse_subscribe_int();

/**
 * @brief Unsubscribes mouse interrupts
 *
 * @return 0 if successful, 1 if not
 */
int mouse_unsubscribe_int();

/**
 * @brief Returns the bitmask for the mouse's hook_id, set when subscribing interrupts.
 *
 * @return Mouse's hook_id bit mask
 */
int get_mouse_hook_id();

/**
 * @brief Reads whatever data is in OUT_BUF and stores it in g_kbd_data.
 *
 * Checks status register's OBF bit, reads from OUT_BUF and stores it in a global
 * variable, in case there's no error. Otherwise puts -1 in g_handler_err.
 */
void mouse_handler();

/**
 * @brief Version of mouse_handler() in assembly language.
 */
unsigned long mouse_assembly_handler();

/**
 * @brief Wrapper to handle mouse interrupts.
 *
 * Calls mouse's handler and stores g_mouse_data in g_packet. Then, if appropriate
 * (when the handler reads 3 bytes) , returns 0.
 *
 * @param cnt the number of packets read, referring to a packet (from 1 to 3)
 * @return 0 after reading a whole packet, 1 if it didn't read the entire packet, and -1
 * in case of an error.
 */
int mouse_wrapper(int *cnt);

/**
 * @brief Sets stream mode and/or enables data reporting
 *
 * Sets stream mode and/or enables data reporting by
 * Issuing a KBC command and then writing to the mouse.
 *
 * @return 0 if successful, -1 if not
 */
int set_stream_mode(int mode);

/**
 * @brief Issues a command to the KBC if IBF is not set.
 *
 * Checks the status register and writes to the KBC's command register if
 * IBG is not set.
 *
 * @param cmd command to write to KBC
 *
 * @return 0 if successful, -1 if not
 */
int issue_kbc_cmd(unsigned char cmd);

/**
 *  @brief Disables data reporting and/or sets remote mode.
 *
 *	Disables data reporting and/or sets remote mode by
 *	issuing a KBC command and then writing to the mouse.
 *
 *  @param mode if 0, function is meant to also set remote mode
 *  @return 0 if successful, -1 if not
 */
int disable_data_reporting(int mode);

/**
 * @brief Reads acknowledgement byte.
 *
 * Reads acknowledgement byte and returns its value. If it's NACK,
 * tries again.
 *
 * @return 0 if ACK, -1 if ERROR
 */
int read_ack_byte();

/**
 * @brief Checks the signal of the coordinates and updates it accordingly.
 *
 * Checks if the x and y values read from the mouse packets are meant to be negative
 * and adjusts their value if necessary.
 *
 * @param x the x coordinate of the mouse relative to its previous one
 * @param y the y coordinate of the mouse relative to its previous one
 */
void check_coordinates(int *x, int *y);

/**
 * @brief Gets the mouse's x coordinate relative to its previous one
 *
 * Checks if the x value read from the mouse packets is meant to be negative
 * and adjusts its value if necessary, returning it.
 *
 * @return x coordinate's displacement
 */
int get_mouse_X();

/**
 * @brief Gets the mouse's y coordinate relative to its previous one
 *
 * Checks if the y value read from the mouse packets is meant to be negative
 * and adjusts its value if necessary, returning it.
 *
 * @return y coordinate's displacement
 */
int get_mouse_Y();

/**
 * @brief Checks if the user is pressing the mouse's left button
 *
 * Check's the first byte of the mouse packet to see if its left button is pressed.
 *
 * @return 0 if true, 1 if not
 */
int checkLDOWN();

/**
 * @brief Checks if the mouse's cursor is out of boundaries and corrects it.
 *
 * @param x the x coordinate of the mouse's cursor
 * @param y the y coordinate of the mouse's cursor
 */
void check_mouse_boundaries(int *x, int *y);


/** @} end of mouse */


#endif /* _LCOM_MOUSE_H_ */
