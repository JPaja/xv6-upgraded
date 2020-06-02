#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

struct group g;


void useradd(char* dir, int uid, char* name, char* login)
{	
	int i;
	if(uid < 0)
	{
		struct user s;
		for(i = 1001; getUser(&s, i) ;i++);
		uid = i;
	}
	struct user s;
	if(getUser(&s, uid))
	{
		printf("Korisnicki id je zauzet\n");
		return;
	}
	s.uid = uid;
	
	
	memset(s.home, 0, USERPATHMAXLEN);
	memmove(s.home, dir, strlen(dir));
	memset(s.password, 0, PASSMAXLEN);
	memset(s.username, 0, USERMAXLEN);
	memmove(s.username, login, strlen(login));
	memset(s.realName, 0, RNAMEMAXLEN);
	memmove(s.realName, name, strlen(name));

	for(i = 1001; getGroup(&g, i) ;i++);
	memset(&g, 0, sizeof(struct group));
	int gid = i;
	s.gid = gid;
	g.gid = gid;
	memset(g.name, 0, USERMAXLEN);
	memmove(g.name, login, strlen(login));
	memset(g.users,0 , GROUPUSERMAXLEN * sizeof(struct user));
	memmove(&g.users[0],&s,sizeof(struct user));
	for(int i = 1; i < GROUPUSERMAXLEN; i++)
		g.users[i].uid = -1;

	if(!addUser(&s))
	{
		printf("Greska pri dodavanju korisnika\n");
		return;
	}
	if(!addGroup(&g))
	{
		printf("Greska pri dodavanju grupe\n");
		//removeUser(&s);
		return;
	}
	int fd = open(s.home,0);
	if(fd < 0)
	{
		if(mkdir(s.home) < 0)
		{
			printf("Greska pri pavljenju home direktorijuma\n");
			//removeUser(&s);
			//removeGroup(&g);
			return;
		}
	}
	close(fd);
	printf("Korisnik je dodat\n");
}
int
main(int argc, char *argv[])
{
	char* dir = 0;
	int uid = -1;
	char* name = 0;
	char* login = 0;
	for(int i = 1; i < argc; i++)
	{
		//printf("%s\n", argv[i]);
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
		else
		{
			if(login)
			{
				printf("Greska: login je vec postavljen\n");
				exit();
			}
			login = argv[i];
		}
	}
	if(!login)
	{
		printf("Greska: morate specifikovati login\n");
		exit();
	}

	char* defPath = "/home/";
	int loginLen = strlen(login);
	int defLen = strlen(defPath);
	char newDir[defLen + loginLen+ 1];

	if(!dir)
	{
		memmove(newDir, defPath, defLen);
		memmove(newDir + defLen, login, loginLen);
		newDir[loginLen + defLen] = '\0';
		dir = newDir;
	}
	char newName[loginLen +1];
	if(!name)
	{
	 	memmove(newName, login, loginLen);
	 	newName[loginLen] = '\0';
		name = newName;
	}

	useradd(dir,uid,name, login);
	exit();
}
