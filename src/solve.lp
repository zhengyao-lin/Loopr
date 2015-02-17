namespace Main {
	.function Recursive(n, m) {
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
		invoke "Recursive", 2

		ldloc n
		ldloc m
		dec
		invoke "Recursive", 2

		add
		ret
	}

	ldb 10
	ldb 10
	invoke "Recursive", 2
	popb
}
