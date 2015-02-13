.function "Recursive", n, m {
	ldloc n
	br.1 {
		ldloc m
		ret
	}

	ldloc m
	br.1 {
		ldloc n
		ret
	}

	ldloc n
	dec
	ldloc m
	call "Recursive", 2

	ldloc n
	ldloc m
	dec
	call "Recursive", 2

	addb
	ret
}

ldb 10
ldb 10
call "Recursive", 2
popb
