
.model	flat

INCLUDELIB LIBCMT

_DATA	SEGMENT

$hello_main		DB	'hello from main()', 0aH, 00H
$rfoo_called	DB	'real_foo() called', 0aH, 00H
$foo_base		DB	'foo() base called', 0aH, 00H


_DATA	ENDS

PUBLIC _main, foo, real_foo, bar

EXTRN	_printf:PROC

_TEXT	SEGMENT

real_foo:

	push OFFSET $rfoo_called
	call _printf
	add esp, 4

	jmp after_foo

foo:

	push OFFSET $foo_base
	call _printf
	add esp, 4

	jmp real_foo


bar:

	push ebp
	mov ebp, esp

	leave
	ret

_main:
	
	push ebp
	mov ebp, esp

	jmp foo
	
	after_foo:

	push OFFSET $hello_main
	call _printf
	add esp, 4

	call bar

	mov eax, 0
	leave
	ret

_TEXT	ENDS

END
