default TestMain

namespace TestMain {
	.function void print(str)
	.function getc()
	.function void println(str) {
		ldarg.0
		ldstr "\n"
		addstr
		invoke print, 1

		ret
	}
	.function void printTAB(str) {
		ldstr "   "
		ldarg.0
		addstr
		invoke println, 1

		ret
	}
	.function void new_line() {
		ldstr "\n"
		invoke print, 1

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

	/****************** start ******************/

	.entry
	.maxstack 1

	.def integer // define a variable
	.def array

	ldstr "---Basic Instruction Test---\n"
	invoke print, 1


	/******** Load ********/
	invoke new_line, 0
	ldstr "LOAD:\n"
	invoke print, 1

	ldstr "   10\t\t= "
	invoke print, 1
	ldb 10 // load byte
	conv.i32.str
	invoke println, 1

	ldstr "   (byte)260\t= "
	invoke print, 1
	ldb.b8 260
	conv.i32.str
	invoke println, 1

	ldstr "   'a'\t\t= "
	invoke print, 1
	ldb 'a'
	conv.i32.str
	invoke println, 1

	ldstr "   true\t\t= "
	invoke print, 1
	ldb true
	conv.i32.str
	invoke println, 1

	ldstr "   false\t= "
	invoke print, 1
	ldb false
	conv.i32.str
	invoke println, 1

	ldstr "   10.500000l\t= "
	invoke print, 1
	ldb 10.500000l
	conv.f16.str
	invoke println, 1

	invoke new_line, 0

	ldstr "   null = "
	invoke print, 1
	ldnull // load null
	invoke println, 1

	ldstr "   Uninitialized local variable = "
	invoke print, 1
	ldloc "integer" // load local variable
	invoke println, 1

	ldstr "   test string: "
	invoke print, 1
	ldstr "This is a test string" // load string
	invoke println, 1

	newarr [10][10][10] // new array
	stloc array // store local variable

	ldloc array
	ldarr [1][2] // load array [1, 2]
	ldb 12
	starr 3 // store array

	ldstr "   array[1][2][3] = "
	invoke print, 1
	ldloc array
	ldarr [1][2][3]
	conv.i32.str
	invoke println, 1

	ldstr "   box&unbox: "
	invoke print, 1
	ldstr "Orignal Data"
	box
	unbox
	invoke println, 1

	/******** Logic ********/
	invoke new_line, 0
	ldstr "LOGIC:\n"
	invoke print, 1

	ldstr "   10 == 11   ... "
	invoke print, 1
	ldb 10
	ldb 11
	eq
	invoke BoolToString, 1
	invoke println, 1

	ldstr "   3.1 == 3.1 ... "
	invoke print, 1
	ldb 3.1
	ldb 3.1
	eqf
	invoke BoolToString, 1
	invoke println, 1

	ldstr "   11 != 12   ... "
	invoke print, 1
	ldb 11
	ldb 12
	ne
	invoke BoolToString, 1
	invoke println, 1

	ldstr "   1 != 1     ... "
	invoke print, 1
	ldb 1
	ldb 1
	ne
	invoke BoolToString, 1
	invoke println, 1

	ldstr "   -2.0 > 1.0 ... "
	invoke print, 1
	ldb -2.0
	ldb 1.0
	gtf
	invoke BoolToString, 1
	invoke println, 1

	ldstr "   12 > 12    ... "
	invoke print, 1
	ldb 12
	ldb 12
	gt
	invoke BoolToString, 1
	invoke println, 1

	ldstr "   13 < 14    ... "
	invoke print, 1
	ldb 12
	ldb 13
	lt
	invoke BoolToString, 1
	invoke println, 1

	ldstr "   12 < 11    ... "
	invoke print, 1
	ldb 12
	ldb 11
	lt
	invoke BoolToString, 1
	invoke println, 1

	invoke new_line, 0

	ldstr "   if (true == true)  ... "
	invoke print, 1
	ldb true
	br.true {
		ldstr "is true"
		invoke println, 1
	}

	ldstr "   if (false == true) ... "
	invoke print, 1
	ldb false
	br.true IF_TRUE
	goto IF_ELSE

IF_TRUE:
	ldstr "is true"
	invoke println, 1
	goto IF_END

IF_ELSE:
	ldstr "is false"
	invoke println, 1
	goto IF_END

IF_END:

	ldstr "   if (5 == (byte)5)  ... "
	invoke print, 1
	ldb 5
	br.5 {
		ldstr "is true"
		invoke println, 1
	}


	/******** Operation ********/
	invoke new_line, 0
	ldstr "OPERATION:\n"
	invoke print, 1

	ldstr "   1 + 5 = "
	invoke print, 1
	ldc.1
	ldc.5
	add
	conv.i32.str
	invoke println, 1

	ldstr "   20 - 352 = "
	invoke print, 1
	ldc.20
	ldb 352
	sub
	conv.i32.str
	invoke println, 1

	ldstr "   16 / 5 = "
	invoke print, 1
	ldc.16
	ldc.5
	div
	conv.i32.str
	invoke println, 1

	ldstr "   20 * 720 = "
	invoke print, 1
	ldc.20
	ldb 720
	mul
	conv.i32.str
	invoke println, 1

	invoke new_line, 0

	ldstr "   (20 + 720) / 3 * 5 + 2015 - 200 = "
	invoke print, 1
	ldc.20
	ldb 720
	add
	ldc.3
	div
	ldc.5
	mul
	ldb 2015
	add
	ldc.200
	sub
	conv.i32.str
	invoke println, 1

	ldstr "   (20.0 + 720.0) / 3.0 * 5.0 + 2015.0 - 200.0 = "
	invoke print, 1
	ldb 20.0
	ldb 720.0
	addf
	ldb 3.0
	divf
	ldb 5.0
	mulf
	ldb 2015.0
	addf
	ldb 200.0
	subf
	conv.f16.str
	invoke println, 1


	/******** Namespace ********/
	invoke new_line, 0
	ldstr "NAMESPACE:\n"
	invoke print, 1

	ldstr "send from TestMain"
	invoke <TestSpace>test, 1

	.using TestSpace
	invoke using_test, 1

	invoke <main>test, 0

	ldstr "\npress a key to exit..."
	invoke print, 1
	invoke getc, 0
}
import test_sub
namespace TestSpace {
	.using TestMain
	.function test(str) {
		ldstr "   namespace TestSpace recieved: "
		ldarg.0
		addstr
		invoke println, 0

		ret
	}
	.function using_test() {
		ldstr "   I'm the one you used..."
		invoke println, 0

		ret
	}
}
