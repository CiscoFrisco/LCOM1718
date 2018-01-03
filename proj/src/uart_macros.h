#ifndef _PROJ_UART_MACROS_H
#define _PROJ_UART_MACROS_H

/** @defgroup uart_macros uart_macros
 * @{
 *
 * Constants for programming the UART.
 */

#define BIT(n) 			(0x01<<(n)) 		/**< @brief constant to get a bit position */

#define UART_IRQ_COM1 	4   				/**< @brief constant to get a bit position */
#define UART_IRQ_COM2 	3   				/**< @brief constant to get a bit position */

#define BASE_ADD 		0x3F8    			/**< @brief RBR/THR Memory Address */


#define IER 			0x3F9     			/**< @brief IER  memory address  */
#define IIR 			0x3FA     			/**< @brief IIR  memory address  */
#define LCR 			0x3FB     			/**< @brief LCR  memory address  */
#define LSR 			0x3FD     			/**< @brief LSR  memory address  */

/* LSR */

#define RD 				BIT(0)  			/**< @brief Receiver Data Bit. If set then there is data for receiving */
#define OE 				BIT(1)  			/**< @brief Overrun Error  Bit. If set then a character received was overwritten by another one */
#define PE 				BIT(2)  			/**< @brief Parity Error Bit. If set then it was received a character with a parity error */
#define FE 				BIT(3) 				/**< @brief Framing Error Bit. If set then the character received does not have a valid Stop bit */
#define BI 				BIT(4)   			/**< @brief Break Interrupt Bit. If set then there is data for receiving */
#define THRE 			BIT(5)  			/**< @brief Transmitter Holding Register Empty  Bit. If set, the UART is ready to accept a new character for transmitting */
#define TER 			BIT(6)   			/**< @brief Transmitter Empty Register Bit. If set, both the THR and the Transmitter Shift Register are empty */

/* IER*/

#define RDA_INT_IER   	BIT(0) 				/**< @brief Enables the Received Data Available Interrupt */
#define THE_INT_IER   	BIT(1) 				/**< @brief Enables the Transmitter Holding Register Empty Interrupt */
#define RLS_INT_IER   	BIT(2) 				/**< @brief Enables the Receiver Line Status Interrupt */

/* IIR */

#define RDA_INT_IIR   	0x4 				/**< @brief  Received Data Available Interrupt */
#define THE_INT_IIR   	0x2 				/**< @brief  Transmitter Holding Register Empty Interrupt */
#define RLS_INT_IIR   	0x6 				/**< @brief  Receiver Line Status Interrupt (FIFO)*/

/* CONFIGURATIONS */
#define LCR_CONF1   	0x9F 				/**< @brief Bitmask for the first of two steps of initializing a configuration with DLA access  */
#define LCR_CONF2   	0xDF 				/**< @brief Bitmask for the last of two steps of initializing a configuration with DLA access */
#define BAUD_RATE   	1200 				/**< @brief Baud Rate */
#define LCR_CONF3   	0x7F 				/**< @brief LCR configuration after setting the bit rate -- permanent conf */
#define IER_CONF    	0x07 				/**< @brief IER configuration -- Enable all interrupts, except MODEM */

#define DIVISOR_LSB 	0x60  				/**< @brief  Divisor Latch Less Significant Byte */
#define DIVISOR_MSB 	0x00  				/**< @brief  Divisor Latch Most Significant Byte */

/**@}  end of uart_macros */

#endif /* _PROJ_UART_MACROS_H */
