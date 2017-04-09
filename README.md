# vtrace
Linux Shellcode Injection Tool
Allows to inject your shellcode to another process memory and execute.
You can return control to the program

# Compiling

gcc vtrace.c proc.c vmap.c -o vtrace

or 32 bit

gcc -m32 vtrace.c proc.c vmap.c -o vtrace

Usage: ./vtrace [pid] [shellcode _file]

# Shellcode writing tips

When VTrace loads the shellcode, into stack pushed:
[HIGH Adress]
1. Return address
2. Old RBX/EBX
[LOW Adress]

Into RBX/EBX storeed libc base pointer, this allow you to use libc stuff with offsets
More examples in shellcode/
