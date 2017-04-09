use32
;ebx+0x127090

push 1
jmp _text
_code:

lea eax,[ebx+0x127090]
call eax
add esp,8

pop ebx
ret

_text:
call _code
db "/home/adriane/sys4proc/libtest.so",0
