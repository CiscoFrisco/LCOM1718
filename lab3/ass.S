/*
Version of read_scan_code() written in assembly.
*/

#include "i8042.h"

.global _g_data, _assemblyHandler

.bss
.lcomm _g_data, 8

.data
_try_cnt: .int 5

.text

_assemblyHandler:

	xor %eax,%eax
	mov _try_cnt, %ecx

in: inb $STAT_REG /* reads the status register */
	movb %al, %bl /* saves stat in %bl */
	test  $OBF,%bl /* testing whether the Output Buffer is full */
	jz next_it /* if it's not, jump to the next iteration */
	inb $OUT_BUF /* if it is, read from the Output Buffer */
	test $(PAR_ERR | TO_ERR), %bl /* Test whether there's parity or timeout errors */
	jnz zero_ret /* if there is, store 0 in g_data (error code) */
	movb %al, _g_data /* if there isn't, store whatever has been read */
	jmp end

next_it: loopnz in /*loop while ecx<try_cnt and OBF is empty*/

zero_ret: movb $0,_g_data

end:  ret
