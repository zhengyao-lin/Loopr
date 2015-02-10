.function "Recursive", 2 {
	ldarg.0
	br.1 {
		ldarg.1
		ret
	}

	ldarg.1
	br.1 {
		ldarg.0
		ret
	}

	ldarg.0
	dec
	ldarg.1
	call "Recursive", 2

	ldarg.0
	ldarg.1
	dec
	call "Recursive", 2

	addb
	ret
}

ldb 10
ldb 10
call "Recursive", 2
popb
