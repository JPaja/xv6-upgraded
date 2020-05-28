#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"


void passwd(char* user)
{
	printf("%s\n",user);
}

int
main(int argc, char *argv[])
{
	char* user = "";
	if(argc == 1)
	{
		printf("Fali korisnicko ime\n");
		exit();
	}
	else if(argc > 2)
	{
		printf("Izaberite samo jedno korisnicko ime\n");
		exit();
	}
	user = argv[1];
	passwd(user);
	exit();
}
