#include "kernel/types.h"
#include "kernel/defs.h"
#include "kernel/param.h"
#include "kernel/traps.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "kernel/memlayout.h"
#include "kernel/mmu.h"
#include "kernel/proc.h"
#include "kernel/x86.h"
#include "kernel/dev.h"

int seed = 0xDA;

int
randomRead(struct inode* ip, char *dst, int n)
{
	for(int i = 0 ; i < n; i++)
	{

		dst[i] = (char)((seed >> 10)& 0x7FFF);
		updateRandomSeed((seed >> 18)& 0xFF);
	}
    return n; // mora da bude n inace panikuje
}

int
randomWrite(struct inode *ip, char *buf, int n)
{
	for(int i = 0; i < n; i++)
	{
		updateRandomSeed(buf[i]);
	}
	return n;
}


//https://stackoverflow.com/a/1280765
void updateRandomSeed(char c)
{
	int i;
	for(i = ticks & 10;i>=0;i--)
		seed = seed*0x343fd+0x269EC3;
}

void
devRandomInit(void)
{
	devsw[DEVRANDOM].write = randomWrite;
	devsw[DEVRANDOM].read = randomRead;
}

