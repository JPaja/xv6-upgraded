#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_clrscr(void)
{
	return 0; 
}

int
sys_getcp(void)
{
	int x;
	int y;
	if(argint(0, &x) < 0 || argint(1, &y) < 0)
		return -1;
	return 0; 
}

int
sys_setcp(void)
{
	int x;
	int y;
	if(argint(0, &x) < 0 || argint(1, &y) < 0)
		return -1;
	return 0; 
}

