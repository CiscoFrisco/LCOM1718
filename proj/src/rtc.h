#ifndef _PROJ_RTC_H_
#define _PROJ_RTC_H_

#include "highscores.h"

/** @defgroup rtc rtc
 * @{
 *
 * Functions related to the rtc module
 */

/* GLOBAL VARIABLES */

unsigned char reg_A; /**< @brief  RTC's Register A */
unsigned char reg_B; /**< @brief  RTC's Register B */


/* FUNCTIONS */

/**
 * @brief Returns the bitmask for the RTC's hook_id, set when subscribing interrupts.
 *
 * @return RTC's hook_id bit mask
 */
int get_rtc_hook_id();

/**
 * @brief Converts a binary coded decimal value to an integer
 * @param value the value to be converted
 * @return value in integer
 */
unsigned int interpret_BCD(unsigned char value);


/**
 * @brief Reads RTC's Register B
 *
 * Reads RTC's Register B and stores it's content in a convenient variable
 */
void read_reg_B();

/**
 * @brief Writes to RTC's Register B.
 */
void write_reg_B();

/**
 * @brief Reads RTC's time registers and returns their converted values in a structure
 *
 * Reads, when appropriate, from RTC's time registers, converts their values
 * to integers and stores them in an appropriate object.
 *
 * @return an object with data related to time
 */
Time read_time();

/**
 * @brief Programs RTC's Register B.
 *
 * Changes RTC's Register B in order to set 24 hours range and data format to BCD.
 */
void programreg_B();

/**
 * @brief Disables RTC's interrupts.
 *
 * Reads RTC's register B, changes it and writes it again.
 */
void disable_interrupts();

/**
 * @brief Enables RTC's interrupts.
 *
 * Reads RTC's register B, changes it and writes it again.
 */
void enable_interrupts();

/**
 * @brief Checks when it's possible to read RTC's time registers.
 *
 * Disables interrupts, reads RTC's Register A and reenables them
 * while UIP bit is set.
 */
void wait_valid_rtc();

/**
 * @brief Assembly function that reads from RTC's time registers.
 *
 * Reads from each RTC time register and stores each value on a global variable.
 */
unsigned long rtc_assembly_handler();

/**
 * @brief Subscribes RTC interrupts
 *
 * Subscribes interrupts, preventing the standard IH from being notified of the occurrence of KBC interrupts.
 *
 * @return 0 is successful, 1 if not
 */
int rtc_subscribe_int();

/**
 * @brief Unsubscribes RTC interrupts
 *
 * @return 0 if successful, 1 if not
 */
int rtc_unsubscribe_int();


/** @} end of rtc */

#endif /* _PROJ_RTC_H_ */
