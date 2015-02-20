default Main
import solve
namespace SubMain {
	.function void non_function() {
		ldstr "you big shit!"
		popstr
		//invoke non_function2, 0
	}
	.function void non_function2() {
		ldstr "yes!!!"
		popstr
		invoke <SubMain2>non_function2, 0
	}
}
namespace SubMain2 {
	.function void non_function2() {
		ldstr "haha!!!"
		popstr
	}
}
namespace Main {
	.entry
	.using SubMain
	.using Solve

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
	.function gets()
	.function void println(str) {
		ldstr "\n"
		ldarg.0
		invoke print, 1

		invoke print, 1
		invoke non_function, 0

		ret
	}

	ldstr "你好"
	invoke println, 1

	/*ldstr "issue a command: "
	invoke print, 1

	ldstr "\""
	invoke gets, 0
	addstr
	ldstr "\"? what dose that mean?\n"
	addstr
	invoke println, 1*/

	.function TRIMUL(first, second, third) {
		.def var
		ldarg.0
		ldarg.1
		ldarg.2
		mul
		mul

		ldstr "this is a bomb!"
		stloc var
		ldloc var
		popstr

		ret
	}

	.function BoolToString(bvalue) {
		ldarg.0
		br.true {
			ldstr "true"
			ret
		}
		ldstr "false"
		ret
	}

	.function void Printk(str) {
		ldarg.0
		popstr
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
		ldstr "hi block"
		popstr
	}

	ldstr "printstr"
	invoke Printk, 1

	/*ldstr "260.0"
	conv.b8
	popb*/


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

	/*ldb.bl true
	conv.str
	popstr*/

	ldb 10
	ldb 10
	ne
	br.true IS_TRUE
	ldstr "it's false!"
	invoke println, 1
	goto END

IS_TRUE:
	ldstr "it's true!"
	invoke println, 1
END:

	ldb 10
	ldb 10
	invoke Recursive, 2
	popb
}
