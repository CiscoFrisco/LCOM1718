#ifndef _LCOM_i8042_H_
#define _LCOM_i8042_H_

//////////Utilities/////////

#define BIT(n) (0x01<<(n))

#define CVT_BREAK(key)   ((key) | BIT(7)) //Converts a given makecode to its corresponding breakcode

#define CVT_MAKE(key)    ((key)  & 0x7F) //Converts a given breakcode to its corresponding makecode

#define ESC_KEY      0x01 //Esc key makecode

#define DELAY_US      20000

#define KBD_IRQ       1

#define OUT_BUF       0x60

#define MSB           BIT(7)

#define TWO_BYTE      0xE0

////////Status Register//////////

#define STAT_REG      0x64

#define OBF           BIT(0)

#define IBF 	      BIT(1)

#define TO_ERR        BIT(6)

#define PAR_ERR       BIT(7)

///////////KBC/////////////

#define INT_KBC      0x01 //First bit of KBC command byte

#define KBC_CMD_REG   0x64

#define WRITE_COMM   0x60

#define READ_COMM    0x20

#endif /* _LCOM_i8042_H */
