.entry
.maxstack 100

goto __TRIMUL_END //jump throw function
__TRIMUL: //Macro, multiply the last three stacks
	mul
	mul
	goto invoke_BACK
__TRIMUL_END:

//Prototype for function

//push arguments
ldb 16384l
ldb 16384l
ldb 16384l

//invoke function
goto __TRIMUL
invoke_BACK: //invoke back address

//get result
popb

.function void hello()
.function void print(str)
.function getc()
.function void println(str) {
	ldstr "\n"
	ldloc str
	invoke print, 1
	invoke print, 1
	ret
}

ldstr "你好"
invoke println, 1
ldstr "issue a character: "
invoke print, 1
//invoke getc, 0
//popb

.function TRIMUL(first, second, third) {
	.def var
	ldloc first
	ldloc second
	ldloc third
	mul
	mul

	ldstr "this is a bomb!"
	stloc var
	ldloc var
	popstr

	ret
}

.function BoolToString(bvalue) {
	ldloc bvalue
	br.true {
		ldstr "true"
		ret
	}
	ldstr "false"
	ret
}

.function Printk(str) {
	ldloc str
	popstr
	ldloc str
	ret
}

LOOP1:

invoke "hello",0

ldb '\033'
popb

ldb true
invoke "BoolToString", 1
popstr

ldb 5
ldb 6
ldb 7
invoke TRIMUL, 3
popb

HI: {
	ldstr "hi"
	popstr
}

ldstr "printstr"
invoke Printk, 1
popstr

ldstr "260.0"
conv.b8
popb
