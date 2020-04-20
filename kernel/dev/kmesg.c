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

#define SIZE 1024
char buffer[1024];
int pos = 0;
int len = 0;


int getKMESGPos()
{
	return pos;
}
int getKMESGLen()
{
	return len;
}
void setKMSEGPos(int position)
{
	pos = position;
}

void putcKMESG(char c)
{
	if(len == SIZE)
	{
		memmove(buffer, buffer + 1, SIZE - 1);
		len--;
		pos--;
	}
	buffer[len] = c;
	len++;
}

int min(int a, int b)
{
	if(a < b)
		return a;
	return b;
}

int
kmesgRead(struct inode *ip, char *dst, int n)
{
	memset(dst,0,n);
	int myN = len - pos;
	if(n < myN)
		myN = n;

	for(int i =0 ; i< myN;i++)
	 	dst[i] = buffer[pos+ i];
	//memmove(buffer, buffer + myN, SIZE - myN);
	pos += myN;
    return n;
}

int
kmesgWrite(struct inode *ip, char *buf, int n)
{
	return -1;
}

void
devKmesgInit(void)
{
	devsw[DEVKMESG].write = kmesgWrite;
	devsw[DEVKMESG].read = kmesgRead;
}

