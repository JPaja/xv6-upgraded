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


int
zeroRead(struct inode *ip, char *dst, int n)
{
    for(int i= 0; i< n;i++)
        dst[i] = 0;
    return n;
}

int
zeroWrite(struct inode *ip, char *buf, int n)
{
	return n;
}

void
devZeroInit(void)
{
	devsw[DEVZERO].write = zeroWrite;
	devsw[DEVZERO].read = zeroRead;
}
