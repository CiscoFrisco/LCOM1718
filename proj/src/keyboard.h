#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

/** @defgroup keyboard keyboard
 * @{
 *
 * Keyboard related functions
 */

unsigned char g_kbd_data; /**< @brief Stores bytes read from the OUT_BUF */

/**
 * @brief Subscribes keyboard interrupts
 *
 * Subscribes interrupts, preventing the standard IH from being notified of the occurrence of KBC interrupts.
 *
 * @return 0 is successful, 1 if not
 */
int kbd_subscribe_int();

/**
 * @brief Unsubscribes keyboard interrupts
 *
 * @return 0 if successful, 1 if not
 */
int kbd_unsubscribe_int();

/**
 * @brief Returns the bitmask for the keyboad's hook_id, set when subscribing interrupts.
 *
 * @return Keyboard's hook_id bit mask
 */
int get_keyboard_hook_id();

/**
 * @brief Reads whatever data is in OUT_BUF by calling kbd_c_handler() and stores it in g_data.
 * @param *type signals whether the function is reading a one (0) or two (1) byte scancode.
 *
 * @return 0 if successful, -1 if not
 */
int kbd_c_wrapper(unsigned int *type);

/**
 * @brief Reads whatever data is in OUT_BUF and stores it in g_data.
 */
void kbd_c_handler();

/**
 * @brief Clears global variable containing keyboard data.
 */
void clear_kbd_data();

/** @} end of keyboard */

#endif /* _LCOM_KEYBOARD_H */
