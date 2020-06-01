#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"
#include "kernel/x86.h"
// #include "kernel/types.h"
// #include "user.h"
// #include "kernel/fs.h"

char*
strcpy(char *s, const char *t)
{
	char *os;

	os = s;
	while((*s++ = *t++) != 0)
		;
	return os;
}

char*
strncpy(char *s, const char *t, int n)
{
	char *os;

	os = s;
	while(n-- > 0 && (*s++ = *t++) != 0)
		;
	while(n-- > 0)
		*s++ = 0;
	return os;
}

// Like strncpy but guaranteed to NUL-terminate.
char*
safestrcpy(char *s, const char *t, int n)
{
	char *os;

	os = s;
	if(n <= 0)
		return os;
	while(--n > 0 && (*s++ = *t++) != 0)
		;
	*s = 0;
	return os;
}

int
strcmp(const char *p, const char *q)
{
	while(*p && *p == *q)
		p++, q++;
	return (uchar)*p - (uchar)*q;
}

int
strstarts(const char *q,const char *p)
{
	while (*p)
	{
		if(*p != *q)
			return 0;
		p++;
		q++;
	}
	return 1;
}

uint
strlen(const char *s)
{
	int n;

	for(n = 0; s[n]; n++)
		;
	return n;
}

void*
memset(void *dst, int c, uint n)
{
	stosb(dst, c, n);
	return dst;
}

char*
strchr(const char *s, char c)
{
	for(; *s; s++)
		if(*s == c)
			return (char*)s;
	return 0;
}

char*
gets(char *buf, int max)
{
	int i, cc;
	char c;

	for(i=0; i+1 < max; ){
		cc = read(0, &c, 1);
		if(cc < 1)
			break;
		buf[i++] = c;
		if(c == '\n' || c == '\r')
			break;
	}
	buf[i] = '\0';
	return buf;
}

int
stat(const char *n, struct stat *st)
{
	int fd;
	int r;

	fd = open(n, O_RDONLY);
	if(fd < 0)
		return -1;
	r = fstat(fd, st);
	close(fd);
	return r;
}

int
atoi(const char *s)
{
	int n;

	n = 0;
	while('0' <= *s && *s <= '9')
		n = n*10 + *s++ - '0';
	return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
	char *dst;
	const char *src;

	dst = vdst;
	src = vsrc;
	while(n-- > 0)
		*dst++ = *src++;
	return vdst;
}




int hasSpaces(char* str, int n)
{
	for(int i = 0; i < n ; i++)
	{
		if(str[i] == ' ')
			return 1;
	}
	return 0;
}


int readLine(char* buffer, int n)
{
	return freadLine(0, buffer, n);
}
int freadLine(int fd, char* buffer, int n)
{
	int r = 0;
	memset(buffer,0,n);
	char c;
	for(int i = 0; i < n - 1; i++)
	{
		if(read(fd,&c,1) == 0)
			break;
		if(c == '\n')
			break;
		r++;
		buffer[i] = c;
	}
	return r;
}

int readUser(struct user* s, char* buff, int n)
{
	int id = 0;
	char b[512];
	int bi = 0;
	memset(b, 0, 512);
	for(int i = 0 ; i< n; i++)
	{
		if(id > 6)
			break; 
		char c = buff[i];
		if(c == ':' )
		{
			if(hasSpaces(b, bi))
			{
    			return 0;//greska text ne sme da ima razmake
			}
			switch (id)
			{
				case 0:
					memmove(s->username,b, bi);
					break;
				case 1:
					memmove(s->password,b, bi);
					break;
				case 2:
					s->uid = atoi(bi);
					break;
				case 3:
					s->gid = atoi(bi);
					break;
				case 4:
					memmove(s->realName,b, bi);
					break;
				case 5:
					memmove(s->home,b, bi);
					break;
			}
			bi = 0;
			id++;
			continue;
		}
		b[bi++] = c;
		b[bi] = 0;

	}

	if(id < 5)
	{
    	return 0;
	}
	if(id == 6 && bi != 0)
		memmove(s->home,b, bi);
	return 1;
}
int readGroup(struct group* g, char* buff, int n)
{
	int id = 0;
	char b[512];
	int bi = 0;
	memset(b, 0, 512);
	for(int i = 0 ; i< n; i++)
	{
		if(id > 6)
			break; 
		char c = buff[i];
		if(c == ':' )
		{
			if(hasSpaces(b, bi))
			{
    			return 0;//greska text ne sme da ima razmake
			}
			struct user u;
			switch (id)
			{
				case 0:
					memmove(g->name,b, bi);
					break;
				case 1:
					g->gid = atoi(bi);
					break;
				default:
					
					if(!getUserByName(&u,bi))
						return 0; //greska pri ucitavanju usera;
					memmove(&g->users[i], &u, sizeof(struct user));
					break;
			}
			bi = 0;
			id++;
			continue;
		}
		b[bi++] = c;
		b[bi] = 0;
	}

	if(id < 2)
	{
    	return 0;
	}
	if(bi != 0)
	{
		struct user u;
		if(!getUserByName(&u,bi))
			return 0; //greska pri ucitavanju usera;
		memmove(&g->users[id], &u, sizeof(struct user));
	}
	return 1;
}


#define MAXUSERS 10
#define MAXGROUPS 10

int getUsers(struct user* u , int max)
{
	memset(u,0,sizeof(&u) * max);
	for(int i = 0; i < MAXUSERS; i++)
		u[i].uid = -1;

	int fdPasswd;
	if((fdPasswd = open("/etc/passwd",0)) < 0)
	{
    	return 0;
	}
	char buff[512];
	int i = 0; 
	for(int i = 0 ; i < max; i++)
	{
		int n = freadLine(fdPasswd,buff, 512);
		if(n == 0)
			break;
		if(!readUser(&u[i], buff, n))
		{
    		return 0;
		}
	}
	return 1;
}

// struct groups** getUsers()


int getGroups(struct group* g , int max)
{
	memset(g,0,sizeof(&g) * max);
	for(int i = 0; i < MAXGROUPS; i++)
		g[i].gid = -1;

	int fdPasswd;
	if((fdPasswd = open("/etc/group",0)) < 0)
	{
    	return 0;
	}
	char buff[512];
	int i = 0; 
	for(int i = 0 ; i < max; i++)
	{
		int n = freadLine(fdPasswd,buff, 512);
		if(n == 0)
			break;
		if(!readGroup(&g[i], buff, n))
		{
			close(fdPasswd);
    		return 0;
		}
	}
	close(fdPasswd);
	return 1;
}

struct user users[MAXUSERS];
struct group groups[MAXGROUPS];

int getUser(struct user* buffer, int uid)
{
	if(!getUsers(users, MAXUSERS))
	 	return 0;
	for(int i = 0; i < MAXUSERS; i++)
	{
		if(users[i].uid == -1)
			break;
		if(users[i].uid == uid)
		{
			memmove(buffer, &users[i],sizeof(struct user));
			return 1;
		}
	}
    return 0;
}
int getUserByName(struct user* buffer, char * name)
{
   	if(!getUsers(users, MAXUSERS))
	 	return 0;
	for(int i = 0; i < MAXUSERS; i++)
	{
		if(users[i].uid == -1)
			break;
		if(strcmp(users[i].username, name) == 0)
		{
			memmove(buffer, &users[i],sizeof(struct user));
			return 1;
		}
	}
    return 0;
}

int loginUser(struct user* user, char * password)
{
    return strcmp(users->password, password) == 0; //TODO: dodati hash passworda
}
int getGroup(struct group* buffer, int gid)
{
    return 0;
}
int getGroupByName(struct group* buffer, char * name)
{
    return 0;
}

int addUser(struct user* user)
{
    return 0;
}
int addGroup(struct group* user)
{
    return 0;
}