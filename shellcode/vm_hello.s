use64

jmp _text
_code:
pop rsi

xor rax,rax
xor rdx,rdx
inc rax
mov rdi,rax
mov dl,13
syscall

pop rbx
ret

_text:
call _code
db "Hello World!",0x0A
