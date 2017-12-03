#include "i8042.h"

.global _assemblyHandler

.data
_try_cnt: .int 5

.text

_assemblyHandler:

	xor %eax,%eax
	mov _try_cnt, %ecx

in: inb $STAT_REG
	movb %al, %bl
	test  $OBF,%bl
	jz next_it
	inb $OUT_BUF
	test $(PAR_ERR | TO_ERR), %bl
	jnz zero_ret
	jmp end

next_it: loopnz in

zero_ret: movb $0,%al

end: ret
