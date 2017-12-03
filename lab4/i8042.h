#ifndef _LCOM_i8042_H_
#define _LCOM_i8042_H_

//////////Utilities/////////

#define BIT(n) (0x01<<(n))

#define CVT_BREAK(key)   ((key) | BIT(7)) //Converts a given makecode to its corresponding breakcode

#define CVT_MAKE(key)    ((key)  & 0x7F) //Converts a given breakcode to its corresponding makecode

#define ESC_KEY      0x01 //Esc key makecode

#define DELAY_US      20000

#define KBD_IRQ       1

#define MOUSE_IRQ     12

#define OUT_BUF       0x60

#define IN_BUF        0x60

#define MSB           BIT(7)

#define TWO_BYTE      0xE0

////////Status Register//////////

#define STAT_REG      0x64

#define OBF           BIT(0)

#define IBF 	      BIT(1)

#define TO_ERR        BIT(6)

#define PAR_ERR       BIT(7)

#define AUX 		  BIT(5)

///////////KBC/////////////

#define INT_KBC       0x01 //First bit of KBC command byte

#define KBC_CMD_REG   0x64

#define WRITE_COMM    0x60

#define READ_COMM     0x20


/////////MOUSE///////////

#define WR_MOUSE     0xD4

#define STREAM_MODE  0xEA

#define DATA_REPORT  0xF4

#define DISABLE      0xF5

#define RM_MODE      0xF0

#define READ_DATA    0xEB

#define DISABLE_INT  0xFD

#define INT_M_KBC    0x02


//////ACKNOWLEDGEMENT BYTE////

#define ACK          0xFA

#define NACK         0xFE

#define ERROR        0xFC

////////PACKET - BYTE 1///////

#define LB_BIT    BIT(0)

#define RB_BIT    BIT(1)

#define MB_BIT    BIT(2)

#define X_SIGN    BIT(4)

#define Y_SIGN    BIT(5)

#define XOV_BIT   BIT(6)

#define YOV_BIT   BIT(7)


#endif /* _LCOM_i8042_H */
