
.model	flat

INCLUDELIB LIBCMT
;INCLUDELIB OLDNAMES

_DATA	SEGMENT

$str1		DB	'hello world', 0aH, 00H
$foo_called	DB 'foo() called', 0aH, 00H

_DATA	ENDS

PUBLIC _main, foo, real_foo, bar

EXTRN	_printf:PROC

_TEXT	SEGMENT

foo:

	push ebp
	mov ebp, esp

real_foo:

	push OFFSET $foo_called
	call _printf
	add esp, 4

	leave
	ret

bar:

	push ebp
	mov ebp, esp

	leave
	ret

_main:
	
	push ebp
	mov ebp, esp

	call foo
	call bar

	;push OFFSET $str1
	;call _printf

	mov eax, 0
	leave
	ret

_TEXT	ENDS

END
