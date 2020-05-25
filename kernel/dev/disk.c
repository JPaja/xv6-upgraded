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
#include "kernel/buf.h"

int offset = 0;

int
diskRead(struct inode *ip, char *dst, int n)
{
	uint tot, m;
	struct buf *bp;
	for(tot=0; tot<n; tot+=m, offset+=m, dst+=m){
		bp = bread(ip->dev, offset/BSIZE);
		m = min(n - tot, BSIZE - offset%BSIZE);
		memmove(dst, bp->data + offset%BSIZE, m);
		brelse(bp);
	}
    return n;
}

int
diskWrite(struct inode *ip, char *buf, int n)
{

	uint tot, m;
	struct buf *bp;
	for(tot=0; tot<n; tot+=m, offset+=m, buf+=m){
		bp = bread(ip->dev, offset/BSIZE);
		m = min(n - tot, BSIZE - offset%BSIZE);
		memmove(bp->data + offset%BSIZE, buf, m);
		log_write(bp);
		brelse(bp);
	}
	return n;
}

void setDiskOffset(int bl)
{
	offset = bl;
}
int getDiskOffset()
{
	return offset;
}


void
devDiskInit(void)
{
	devsw[DEVDISK].write = diskWrite;
	devsw[DEVDISK].read = diskRead;
}

