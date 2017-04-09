use32

xor eax,eax
xor ebx,ebx
xor edx,edx
jmp _text
_code:
pop ecx

mov al,4
mov dl,13
inc ebx
int 0x80

pop ebx
ret

_text:
call _code
db "Hello World!",0x0A
