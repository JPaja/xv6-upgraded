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
diskRead(struct inode *ip, char *dst, int n)
{
    return n; // mora da bude n inace panikuje
}

int
diskWrite(struct inode *ip, char *buf, int n)
{
	return n;
}

void
devDiskInit(void)
{
	devsw[DEVDISK].write = diskWrite;
	devsw[DEVDISK].read = diskRead;
}
