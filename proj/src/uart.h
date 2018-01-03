#ifndef _PROJ_UART_H
#define _PROJ_UART_H

/** @defgroup uart uart
 * @{
 *
 * Functions for programming the UART.
 */

/**
 * @brief Reads the content of the RBR and puts it in the g_uart_rbr_data variable
 */
void clearRBR();


/**
 * @brief Subscribes and enables UART interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int subscribe_uart();


/**
 * @brief Unsubscribes UART interrupts
 *
 * @return Returns 0 upon success and negative value on failure
 */
int unsubscribe_uart();


/**
 * @brief Sets the configuration of the LCR
 */
void setup_LCR();


/**
 * @brief Sets the configuration of the IER
 */
void setup_IER();


/**
 * @brief UART Interrupt Handler
 *
 * Handles different kinds of interrupts
 */
void uart_handler();

/**
 * @brief Version of uart_handler in Assembly.
 */
unsigned long uart_assembly_handler();

/**
 * @brief Check whether or not the RBR has data for receiving
 *
 * @return Returns 0 if there's data available in the RBR, otherwise returns 1
 */
int check_RBR();


/**
 * @brief Checks whether or not the THR is empty
 *
 * @return Returns 0 if it the THR is empty, otherwise returns 1
 */
int check_THEmpty();


/**
 * @brief Writes to the THR if empty
 *
 * @param data Data to be sent to the THR
 */
void send_data(unsigned char data);


/**
 * @brief Gets the UART's hook id
 *
 * @return Returns bit order in interrupt mask
 */
int get_uart_hook_id();

/**
 * @brief Clears global variable containing received data.
 */
void clear_uart_data();

/** @} end of uart */


#endif /* _PROJ_UART_H */
