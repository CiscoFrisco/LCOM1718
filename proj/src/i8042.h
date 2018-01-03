#ifndef _LCOM_i8042_H_
#define _LCOM_i8042_H_

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard Controler.
 */

//////////Utilities/////////

#define BIT(n) 			 (0x01<<(n))  					/**< @brief constant to get a bit position */

#define CVT_BREAK(key)   ((key) | BIT(7)) 				/**< @brief Converts a given makecode to its corresponding breakcode */

#define CVT_MAKE(key)    ((key)  & 0x7F) 				/**< @brief Converts a given breakcode to its corresponding makecode */

#define ESC_KEY      	 0x01 							/**< @brief Esc key makecode */

#define DELAY_US      	 20000 							/**< @brief Delay */

#define KBD_IRQ       	 1 								/**< @brief Keyboard IRQ */

#define MOUSE_IRQ     	 12 							/**< @brief Mouse IRQ */

#define OUT_BUF       	 0x60 							/**< @brief Memory address of the Output Buffer */

#define IN_BUF        	 0x60 							/**< @brief Memory address of the Input Buffer */

#define MSB           	 BIT(7) 						/**< @brief Most Significant Bit */

#define TWO_BYTE      	 0xE0 							/**< @brief In case we have a two byte makecode */

#define A_MAKECODE    	 0x1E 							/**< @brief 'A' key makecode */

#define S_MAKECODE    	 0x1F 							/**< @brief 'S' key makecode */

#define D_MAKECODE    	 0x20 							/**< @brief 'D' key makecode */

#define W_MAKECODE    	 0x11 							/**< @brief 'W' key makecode */

#define ENTER_MAKECODE 	 0x1C 							/**< @brief Enter key makecode */

#define SPACE_MAKECODE 	 0x39 							/**< @brief Space key makecode */

////////Status Register//////////

#define STAT_REG      	 0x64 							/**< @brief Memory address of the Status Register */

#define OBF           	 BIT(0) 						/**< @brief Output Buffer Full Bit. Set if Output Buffer Full */

#define IBF 	      	 BIT(1) 						/**< @brief Output Buffer Full Bit. Set if Input Buffer Full */

#define TO_ERR        	 BIT(6) 						/**< @brief TimeOut Error Bit */

#define PAR_ERR       	 BIT(7) 						/**< @brief Parity Error Bit */

#define AUX 		  	 BIT(5) 						/**< @brief Parity Error Bit. Indicates whether the data in the OUT_BUF is coming from the Mouse or the Keyboard */

///////////KBC/////////////

#define INT_KBC       	 0x01 							/**< @brief First bit of KBC command byte */

#define KBC_CMD_REG   	 0x64 							/**< @brief KBC Memory Adress */

#define WRITE_COMM    	 0x60							/**< @brief Command to write to the KBC */

#define READ_COMM     	 0x20 							/**< @brief Command to read the KBC */

/////////MOUSE///////////

#define WR_MOUSE     	 0xD4 							/**< @brief Command to write to the Mouse */

#define STREAM_MODE  	 0xEA 							/**< @brief Set stream mode command */

#define DATA_REPORT  	 0xF4 							/**< @brief Enable data reporting command */

#define DISABLE      	 0xF5 							/**< @brief Disable data reporting command */

#define RM_MODE      	 0xF0 							/**< @brief Set remote mode command */

#define READ_DATA    	 0xEB 							/**< @brief Read Data */

#define DISABLE_INT  	 0xFD							/**< @brief Disable interrupts */

#define INT_M_KBC    	 0x02  							/**< @brief Bit 1 of KBC command byte */

//////ACKNOWLEDGEMENT BYTE////

#define ACK          	 0xFA  							/**< @brief Acknowledgment byte sent by the controller if everything is OK */

#define NACK         	 0xFE  							/**< @brief Acknowledgment byte sent by the controller if invalid byte */

#define ERROR        	 0xFC  							/**< @brief Acknowledgment byte sent by the controller if there's a second consecutive invalid byte */

////////PACKET - BYTE 1///////

#define LB_BIT    	 	 BIT(0) 						/**< @brief Left Hand Mouse Button Bit. Set if left hand mouse button is pressed */

#define RB_BIT    	 	 BIT(1) 						/**< @brief Right Hand Mouse Button Bit. Set if right hand mouse button is pressed */

#define MB_BIT    		 BIT(2) 						/**< @brief Middle Mouse Button Bit. Set if middle mouse button is pressed */

#define X_SIGN    		 BIT(4) 						/**< @brief Sign of the Mouse's X coordinate . Set if negative */

#define Y_SIGN    		 BIT(5) 						/**< @brief Sign of the Mouse's Y coordinate. Set if negative */

#define XOV_BIT   		 BIT(6) 						/**< @brief X coordinate overflow Bit. Set if X coordinate overflows */

#define YOV_BIT   		 BIT(7) 						/**< @brief Y coordinate overflow Bit. Set if Y coordinate overflows */

/**@} end of i8042 */

#endif /* _LCOM_i8042_H */
