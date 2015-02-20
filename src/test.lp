default Test_Main
namespace Test_Main {
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

	ldstr "   120 == 120 ... "
	invoke print, 1
	ldb 120
	ldb 120
	eq
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

	ldstr "   12 > 13    ... "
	invoke print, 1
	ldb 12
	ldb 13
	gt
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

	ldstr "\npress a key to exit..."
	invoke print, 1
	invoke getc, 0
}
