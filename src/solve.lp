.maxstack 100
.function "Recursive", 2 {
	ldarg.1
	br.1 {
		ldarg.2
		ret
	}

	ldarg.2
	br.1 {
		ldarg.1
		ret
	}

	ldarg.1
	dec
	ldarg.2
	call "Recursive", 2

	ldarg.1
	ldarg.2
	dec
	call "Recursive", 2

	addb
	ret
}

ldb 10
ldb 10
call "Recursive", 2
popb
/*.function "test", 2 {
	ldarg.1
	popb

	ldarg.2
	popb
	ret
}

ldb 123
ldb 234
call "test", 2*/
