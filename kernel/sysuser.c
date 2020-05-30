//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

int
sys_getuid(void)
{
	struct proc * p = myproc();
	if(p == 0)
		return -1;
	return p->uid;
}

int
sys_geteuid(void)
{
		struct proc * p = myproc();
	if(p == 0)
		return -1;
	return p->euid;
}

int
sys_setuid(void)
{
	int uid;

	if(argint(0, &uid) < 0)
		return -1;

	return -1;
}

int
sys_setgroups(void)
{
	int ngroups;
	int* grids;

	if(argint(0, &ngroups) < 0 || argint(1, &grids) < 0)
		return -1;

	return -1;
}

int
sys_chmod(void)
{
	char* pathname;
	int mode;

	if(argint(0, &pathname) < 0 || argint(1, &mode) < 0)
		return -1;

	return -1;
}

int
sys_chown(void)
{
	char* pathname;
	int owner, group;

	if(argint(0, &pathname) < 0 || argint(1, &owner) < 0 || argint(2, &group) < 0)
		return -1;

	return -1;
}