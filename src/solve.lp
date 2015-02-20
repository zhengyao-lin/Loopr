namespace Solve {
	.function Recursive {
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
		invoke "Recursive", 2

		ldarg.0
		ldarg.1
		dec
		invoke "Recursive", 2

		add
		ret
	}

	ldc.10
	ldc.10
	invoke "Recursive", 2
	popb
}
