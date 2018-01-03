#include "i8042.h"

.global _g_mouse_data, _g_handler_err, _mouse_assembly_handler

.bss
.lcomm _g_mouse_data  8
.lcomm _g_handler_err 8
.data
_try_cnt: .int 5

.text

_mouse_assembly_handler:

			xor %eax,%eax
			mov _try_cnt, %ecx

in:  		inb $STAT_REG 					/* reads the status register */
	 		movb %al, %bl 					/* saves stat in %bl */
     		test $OBF,%bl
     		jz next_it
     		test $AUX,%bl
     		jz next_it
     		inb $OUT_BUF 					/* if it is, read from the Output Buffer */
	 		test $(PAR_ERR | TO_ERR), %bl 	/* Test whether there's parity or timeout errors */
     		jnz err
     		movb %al, _g_mouse_data
     		movb $0, _g_handler_err
     		jmp end

next_it: 	loopnz in 						/*loop while ecx<try_cnt and OBF is empty*/

err: 		movb $-1, _g_handler_err

end: 		ret
