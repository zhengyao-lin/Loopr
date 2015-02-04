.maxstack 3
.entry

jmp __TRIMUL_END //jump throw function
__TRIMUL: //Macro, multiply the last three stacks
	mulb
	mulb
	jmp CALL_BACK
__TRIMUL_END:

//Prototype for function

//push arguments
ldb 16384l
ldb 16384l
ldb 16384l

//call function
jmp __TRIMUL
CALL_BACK: //call back address

//get result
popb

//exit
exit
