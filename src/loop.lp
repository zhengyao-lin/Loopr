.maxstack 2
.entry

ldb 2048
ldb 2048
jmp _MUL_
CALLER:
popb
exit

jmp _MUL__END
_MUL_: /* Macro, multiply the last two stacks */
	mulb
	jmp CALLER
_MUL__END:

/*.macro CALLEE func
{
	jmp $1
}*/
