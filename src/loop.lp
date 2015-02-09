.entry

goto __TRIMUL_END //jump throw function
__TRIMUL: //Macro, multiply the last three stacks
	mulb
	mulb
	goto CALL_BACK
__TRIMUL_END:

//Prototype for function

//push arguments
ldb 16384l
ldb 16384l
ldb 16384l

//call function
goto __TRIMUL
CALL_BACK: //call back address

//get result
popb

.function "TRIMUL",3 {
	initloc.i32 int32
	ldarg.0
	ldarg.1
	ldarg.2
	mulb
	mulb
	ldarg.3
	popstr

	ret
}

.function "BoolToString",1 {
	ldarg.0
	br.true {
		ldstr "true"
		ret
	}
	ldstr "false"
	ret
}

LOOP1:

ldb '\033'
popb

ldb true
call "BoolToString", 1
popstr

ldb 5
ldb 6
ldb 7
call TRIMUL, 3
ldb 8
ldb 9
call TRIMUL, 3
popb
//goto LOOP1

EXIT:
//exit
ret
