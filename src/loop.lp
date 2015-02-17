default Main
namespace SubMain {
	.function void non_function() {
		ldstr "hello, world!!!"
		popstr
	}
}
namespace Main {
	.entry

	goto __TRIMUL_END //jump throw function
	__TRIMUL: //Macro, multiply the last three stacks
		mul
		mul
		goto CALL_BACK
	__TRIMUL_END:

	//Prototype for function

	//push arguments
	ldb 16384l
	ldb 16384l
	ldb 16384l

	//invoke function
	goto __TRIMUL
	CALL_BACK: //invoke back address

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
	invoke <SubMain>non_function, 0
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

	.function void Printk(str) {
		ldloc str
		popstr
		ret
		//.function void non_function(){}
	}

	.function void non_function(){}

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
		ldstr "hi block"
		popstr
	}

	ldstr "printstr"
	invoke Printk, 1

	ldstr "260.0"
	conv.b8
	popb


	/* ARRAY! */

	//newarr [10][2][10]

	newarr [2,3,4]

	.def arrayv
	stloc arrayv
	ldloc arrayv

	/*ldb 10
	ldb 10
	ldb 10
	ldb 100
	newarr.4*/

	ldarr [1][2]
	ldb.f8 3.141592653l
	starr 3

	ldloc arrayv
	ldarr [1][2][3]
	popf

	ldb.bl true
	conv.str
	popstr
}
