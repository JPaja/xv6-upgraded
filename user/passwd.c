#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"


void passwd(char* user)
{
	struct user u;
	if(user == 0)
	{
		int uid = getuid();
		if(!getUser(&u, uid))
		{
			printf("Greska pri ucitavanju trenutnog korisnika\n");
			return;
		}
	}
	else
	{
		if(!getUserByName(&u, user))
		{
			printf("Greska pri ucitavanju korisnika\n");
			return;
		}
	}
	char password[512];
	printf("Nova sifra: ");
	memset(password, 0, sizeof(password));
	setecho(0);
	readLine(password, sizeof(password));
	setecho(1);
	printf("\n");

	memmove(u.password, password, PASSMAXLEN);

	if(!updateUser(&u))
	{
		printf("Greska pri izmeni sifre\n");
		return;
	}
	printf("Sifra promenjena\n");
}

int
main(int argc, char *argv[])
{
	char* user = 0;
	if(argc == 2)
	{
		user = argv[1];
	}
	else if(argc > 2)
	{
		printf("Izaberite samo jedno korisnicko ime\n");
		exit();
	}
	passwd(user);
	exit();
}
