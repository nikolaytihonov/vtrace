use64

jmp _text
_code:
pop rdi

xor rsi,rsi
inc rsi
lea rax,[rbx+0x125320]
call rax

pop rbx
ret

_text:
call _code
db "/home/adriane/sys4proc/libtest.so",0
