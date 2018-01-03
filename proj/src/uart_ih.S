#include "uart_macros.h"

.global _uart_assembly_handler, _g_uart_rbr_data

.bss
.lcomm _g_uart_rbr_data 8

.text

_uart_assembly_handler:

          movw $IIR, %dx
          inb %dx
          shlb $4,%al
          shrb $4,%al
          movb $RDA_INT_IIR, %bl
          cmpb %al, %bl
          jne test2

checkRBR: movw $LSR, %dx
          inb %dx
		  mov $RD, %bl
          testb  %al, %bl
          je end
          movw $BASE_ADD, %dx
          inb %dx
          movb %al, _g_uart_rbr_data
          jmp end
test2:	  movb $RLS_INT_IIR, %bl
          cmpb %al, %bl
          jne end
          movw $LSR, %dx
          inb %dx

end:      ret
