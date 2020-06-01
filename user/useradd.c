#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

void useradd(char* dir, int uid, char* name, char* login)
{	

	if(uid < 0)
	{
		struct user s;
		int i;
		for(i = 1001; getUser(&s, i) ;i++);
		uid = i;
	}
	struct user s;
	if(getUser(&s, uid))
	{
		printf("Korisnicki id je zauzet");
		return;
	}
	s.uid = uid;
	s.gid = -1;
	memset(s.home, 0, USERPATHMAXLEN);
	memmove(s.home, dir, strlen(dir));
	memset(s.password, 0, PASSMAXLEN);
	memset(s.username, 0, USERMAXLEN);
	memmove(s.username, login, strlen(login));
	memset(s.realName, 0, RNAMEMAXLEN);
	memmove(s.realName, name, strlen(name));
	if(!addUser(&s))
	{
		printf("Greska pri dodavanju korisnika");
		return;
	}
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
