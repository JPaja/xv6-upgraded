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
