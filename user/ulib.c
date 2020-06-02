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
					s->uid = atoi(b);
					break;
				case 3:
					s->gid = atoi(b);
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
	if(id == 5 && bi != 0)
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
		char c = buff[i];
		if(c == ':' )
		{
			if(hasSpaces(b, bi))
    			return 0;//greska text ne sme da ima razmake
			struct user u;
			switch (id)
			{
				case 0:
					memmove(g->name,b, bi);
					break;
				case 1:
					g->gid = atoi(b);
					break;
				default:
					
					if(!getUserByName(&u,b))
					{
						return 0; //greska pri ucitavanju usera;
					}
					memmove(&g->users[id - 2], &u, sizeof(struct user));
					break;
			}
			bi = 0;
			id++;
			continue;
		}
		b[bi++] = c;
		b[bi] = 0;
	}

	if(id < 1){
    	return 0;
	}
	if(id == 1 && bi != 0)
	{
		g->gid = atoi(b);
	}
	else if(id == 2 && bi != 0)
	{ 
		struct user u;
		if(!getUserByName(&u,b))
		{
			return 0; //greska pri ucitavanju usera;
		}
		memmove(&g->users[id - 2], &u, sizeof(struct user));
	}
	return 1;
}


#define MAXUSERS 10
#define MAXGROUPS 10

int getUsers(struct user* u , int max)
{
	memset(u,0,sizeof(struct user) * max);
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
	close(fdPasswd);
	return 1;
}

// struct groups** getUsers()


int getGroups(struct group* g , int max)
{
	memset(g,0,sizeof(struct group) * max);
	for(int i = 0; i < MAXGROUPS; i++)
	{
		g[i].gid = -1;
		for (int j = 0; j < GROUPUSERMAXLEN; j++)
		{
			g[i].users[j].uid = -1;
		}
	}

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


int writeUsers(struct user* u , int max)
{
	unlink("/etc/passwd");
	int fdPasswd;
	if((fdPasswd = open("/etc/passwd",O_CREATE | O_RDWR)) < 0)
	{
    	return 0;
	}
	int ii =0;
	for(int i = 0 ; i < max; i++)
	{
		if(u[i].uid < 0)
			continue;
		if(ii++ != 0)
			fprintf(fdPasswd,"\n");
		fprintf(fdPasswd, "%s:%s:%d:%d:%s:%s", u[i].username,u[i].password,u[i].uid,u[i].gid,u[i].realName,u[i].home);
	}
	close(fdPasswd);
	return 1;
}

int j;
int writeGroups(struct group* g , int max)
{
	unlink("/etc/group");
	int fdGroup;
	if((fdGroup = open("/etc/group",O_CREATE | O_RDWR)) < 0)
	{
    	return 0; 
	}
	int ii =0;
	for(int i = 0 ; i < max; i++)
	{
		if(g[i].gid < 0)
			continue;
		if(ii++ != 0)
			fprintf(fdGroup,"\n");
		fprintf(fdGroup, "%s:%d", g[i].name,g[i].gid);
		for(j = 0; j < GROUPUSERMAXLEN; j++)
		{
			if(g[i].users[j].uid < 0)
				continue;
			fprintf(fdGroup, ":%s", g[i].users[j].username);
		}
	}
	close(fdGroup);
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
	memset(buffer,0,sizeof(struct user));
   	if(!getUsers(users, MAXUSERS))
	   {
	 	return 0;
	   }
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

int updateUser(struct user* user)
{
	if(!getUsers(users, MAXUSERS))
	 	return 0;
	for(int i = 0; i < MAXUSERS;i++)
	{
		if(users[i].uid == user->uid)
		{
			memmove(&users[i], user, sizeof(struct user));
			writeUsers(users, MAXUSERS);
			return 1;
		}
	}
	return 0;
}
int addUser(struct user* user)
{
	if(!getUsers(users, MAXUSERS))
	 	return 0;
	for(int i = 0; i < MAXUSERS;i++)
	{
		if(users[i].uid < 0){
			memmove(&users[i], user, sizeof(struct user));
			writeUsers(users, MAXUSERS);
			return 1;
		}
	}
	return 0;
}
int removeUser(struct user* user)
{
	if(!getUsers(users, MAXUSERS))
	 	return 0;
	for(int i = 0; i < MAXUSERS;i++)
	{
		if(users[i].uid == user->uid)
		{
			users[i].uid  = -1;
			writeUsers(users, MAXUSERS);
			return 1;
		}
	}
	return 0;
}
int rstrcmp(char * s1, char * s2)
{
	int l1 = strlen(s1);
	int l2 = strlen(s2);
	if(l1 != l2)
		return 0;
	for(int i =0 ; i < l1; i++)
	{
		if(s1[i] != s2[i])
			return 0;
	}
	return 1;
}

int loginUser(struct user* user, char * password)
{
    return rstrcmp(user->password, password); //TODO: dodati hash passworda
}
int getGroup(struct group* buffer, int gid)
{
    if(!getGroups(groups, MAXGROUPS))
	 	return 0;
	for(int i = 0; i < MAXGROUPS; i++)
	{
		if(groups[i].gid == -1)
			break;
		if(groups[i].gid == gid)
		{
			memmove(buffer, &groups[i],sizeof(struct group));
			return 1;
		}
	}
    return 0;
}
int getGroupByName(struct group* buffer, char * name)
{
     if(!getGroups(groups, MAXGROUPS))
	 	return 0;
	for(int i = 0; i < MAXGROUPS; i++)
	{
		if(groups[i].gid == -1)
			break;
		if(strcmp(groups[i].name, name) == 0)
		{
			memmove(buffer, &groups[i],sizeof(struct group));
			return 1;
		}
	}
    return 0;
}


int updateGroup(struct group* group)
{
	if(!getGroups(groups, MAXGROUPS))
	 	return 0;
	for(int i = 0; i < MAXGROUPS;i++)
	{
		if(groups[i].gid == group->gid)
		{
			memmove(&groups[i], group,sizeof(struct group));
			writeGroups(groups, MAXGROUPS);
			return 1;
		}
	}
	return 0;
}
int addGroup(struct group* group)
{
	if(!getGroups(groups, MAXGROUPS))
	{
	 	return 0;
	}
	for(int i = 0; i < MAXGROUPS;i++)
	{
		if(groups[i].gid < 0)
		{
			memmove(&groups[i], group,sizeof(struct group));
			writeGroups(groups, MAXGROUPS);
			return 1;
		}
	}
	return 0;
}
int removeGroup(struct group* group)
{
	if(!getGroups(groups, MAXGROUPS))
	 	return 0;
	for(int i = 0; i < MAXGROUPS;i++)
	{
		if(groups[i].gid  == group->gid)
		{
			groups[i].gid = -1;
			writeGroups(groups, MAXGROUPS);
			return 1;
		}
	}
	return 0;
}