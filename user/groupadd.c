#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

struct group g;


void groupadd(int gid, char* name)
{	
	int i;
	if(gid < 0)
	{
		for(i = 1001; getGroup(&g, i) ;i++);
		gid = i;
	}

	if(getGroup(&g, gid))
	{
		printf("Group id je zauzet\n");
		return;
	}

	memset(&g, 0, sizeof(struct group));
	g.gid = gid;
	memset(g.name, 0, USERMAXLEN);
	memmove(g.name, name, strlen(name));
	memset(g.users,0 , GROUPUSERMAXLEN * sizeof(struct user));
	for(int i = 0; i < GROUPUSERMAXLEN; i++)
		g.users[i].uid = -1;

	if(!addGroup(&g))
	{
		printf("Greska pri dodavanju grupe\n");
		return;
	}
	printf("Grpupa je dodata\n");
}
int
main(int argc, char *argv[])
{

	int gid = -1;
	char* name = 0;
	for(int i = 1; i < argc; i++)
	{
		 if(!strcmp(argv[i], "-g"))
		{
			if(gid >= 0)
			{
				printf("Greska: -g je vec posatvljen\n");
				exit();
			}
			if(i + 1 == argc)
			{
				printf("Greska: fali -g argument\n");
				exit();
			}
			gid = atoi(argv[i + 1]);
			if(gid < 0)
			{
				printf("Greska: -g uind ne sme da bude manji od 0\n");
				exit();
			}
			i++;
		}
		else
		{
			if(name)
			{
				printf("Greska: ime je vec postavljeno\n");
				exit();
			}
			name = argv[i];
		}
	}
	if(!name)
	{
		printf("Greska: morate specifikovati ime\n");
		exit();
	}


	groupadd(gid, name);
	exit();
}