.entry
.maxstack 100

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

.function "TRIMUL", first, second, third {
	.def var
	ldloc first
	ldloc second
	ldloc third
	mulb
	mulb

	ldstr "this is a bomb!"
	stloc var
	ldloc var
	popstr

	ret
}

.function "BoolToString", bvalue {
	ldloc bvalue
	br.true {
		ldstr "true"
		ret
	}
	ldstr "false"
	ret
}

.function "Printk", str {
	ldloc str
	popstr
	ldloc str
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
popb
//goto LOOP1

ldstr "printstr"
call Printk, 1
popstr
