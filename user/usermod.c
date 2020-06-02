
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

struct group g;
struct group gr[MAXGROUPS];

void usermod(char* login, int uid ,char* name, char* dir,int m,char* groups, int a, char* ulogin )
{	
	struct user u;
	struct user t;
	struct user p;
	memset(&u, 0, sizeof(struct user));
	memset(&t, 0, sizeof(struct user));
	memset(&p, 0, sizeof(struct user));
	if(!getUserByName(&u, ulogin))
	{
		printf("Korisnik ne posotji\n");
		return;
	}
	memmove(&t,&u, sizeof(struct user));
	if(uid >= 0 && uid != u.uid)
	{
		if(getUser(&p,uid))
		{
			printf("Izabrani uid vec posotji\n");
			return;
		}
		t.uid = uid;
	}
	if(login)
	{
		if(getUserByName(&p,login))
		{
			printf("Izabrani username vec posotji\n");
			return;
		}
		memset(t.username, 0, USERMAXLEN);
		memmove(t.username, login, strlen(login));
	}
	if(name)
	{
		memset(t.realName, 0, RNAMEMAXLEN);
		memmove(t.realName, name, strlen(name));
	}
	if(dir)
	{
		if(m)
		{
			//TODO: move dir
		}
		int fd = open(dir,0);
		if(fd < 0)
		{
			if(mkdir(dir) < 0)
			{
				printf("Greska pri pavljenju novog home direktorijuma\n");
				return;
			}
			memset(t.home, 0, USERPATHMAXLEN);
			memmove(t.home, dir, strlen(dir));
		}
		close(fd);
	}
	if(groups)
	{
		if(a)
		{
			int n = 0;
			memset(gr,0,MAXGROUPS * sizeof(struct group));
			getGroupsByUser(gr,&n,&u);
			for(int i = 0; i < n; i++)
			{
				for(int j = 0; j < GROUPUSERMAXLEN; j++)
				{
					if(gr[i].users[j].uid == u.uid)
						gr[i].users[j].uid = -1;
				}
				if(!updateGroup(&gr[i]))
				{
					printf("Greska pri otklanjanju grupa\n");
					return;
				}
			}
		}
		char buffer[512];
		int i = 0;
		memset(buffer, 0, sizeof(buffer));
		while (*groups && *groups != '\n' && *groups != ' ')
		{
			if(*groups == ',')
			{
				if(!getGroupByName(&g,buffer))
				{
					printf("Greska: Nije pronadjena grupa\n");
					return;
				}
				if(!addGroupUser(&g,&t))
				{
					printf("Greska pri dodavanju grupe\n");
					return;
				}
				memset(buffer, 0, sizeof(buffer));
				i = 0;
			}
			else
			{
				buffer[i++] = *groups;
			}
		}
	}
	if(!removeUser(&u))
	{
		printf("Greska pri brisanju korisnika\n");
			return;
	}
	if(!addUser(&t))
	{
		printf("Greska pri dodavanju korisnika\n");
			return;
	}
}
int
main(int argc, char *argv[])
{
	char* login = 0;
	int uid = -1;
	char* name = 0;
	char* dir = 0;
	int m = 0;
	char* groups = 0;
	int a = 0;
	char* ulogin = 0;

	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-d"))
		{
			if(dir)
			{
				printf("Greska: -d direktorijum je vec posatvljen\n");
				exit();
			}
			if(i + 1 == argc)
			{
				printf("Greska: fali -d argument\n");
				exit();
			}
			dir = argv[i + 1];
			i++;
		}
		else if(!strcmp(argv[i], "-u"))
		{
			if(uid >= 0)
			{
				printf("Greska: -u je vec posatvljen\n");
				exit();
			}
			if(i + 1 == argc)
			{
				printf("Greska: fali -u argument\n");
				exit();
			}
			uid = atoi(argv[i + 1]);
			if(uid < 0)
			{
				printf("Greska: -u uind ne sme da bude manji od 0\n");
				exit();
			}
			i++;
		}
		else if(!strcmp(argv[i], "-c"))
		{
			if(name)
			{
				printf("Greska: -c ime je vec posatvljen\n");
				exit();
			}
			if(i + 1 == argc)
			{
				printf("Greska: fali -d argument\n");
				exit();
			}
			name = argv[i + 1];
			i++;
		}
		else if(!strcmp(argv[i], "-l"))
		{
			if(login)
			{
				printf("Greska: -l ime je vec posatvljen\n");
				exit();
			}
			if(i + 1 == argc)
			{
				printf("Greska: fali -l argument\n");
				exit();
			}
			login = argv[i + 1];
			i++;
		}
		else if(!strcmp(argv[i], "-m"))
		{
			if(m)
			{
				printf("Greska: -m ime je vec posatvljen\n");
				exit();
			}
			m = 1;			
		}
		else if(!strcmp(argv[i], "-G"))
		{
			if(groups)
			{
				printf("Greska: -G ime je vec posatvljen\n");
				exit();
			}
			if(i + 1 == argc)
			{
				printf("Greska: fali -G argument\n");
				exit();
			}
			groups = argv[i + 1];
			i++;
		}
		else if(!strcmp(argv[i], "-a"))
		{
			if(a)
			{
				printf("Greska: -a ime je vec posatvljen\n");
				exit();
			}
			a = 1;			
		}
		else
		{
			if(ulogin)
			{
				printf("Greska: login je vec postavljen\n");
				exit();
			}
			ulogin = argv[i];
		}
	}
	if(!ulogin)
	{
		printf("Greska: morate specifikovati login\n");
		exit();
	}

	usermod(login,uid,name, dir, m, groups, a, ulogin);
	exit();
}
