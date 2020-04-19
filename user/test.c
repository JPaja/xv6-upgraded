#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


void TestConsoleSystemCalls()
{
	int x;
	int y;
	clrscr();
	getcp(&x,&y);
	printf("Console Test: Ovo se nalazi na vrhu ekrana na poziciji %d %d\n",x,y);
	setcp(10,11);
	getcp(&x,&y);
	printf("Ovo se nalazi na sredini ekrana na poziciji %d, %d\n",x,y);
	
	printf("Test");
	getcp(&x,&y);
	printf("\nPozicija nakon proslog stringa: %d, %d\n",x,y);

}

int checkBytes(char * data, int check, int size)
{
	for(int i = 0; i < size; i++)
		if(data[i] != check)
			return 0;
	return 1;
}

void TestNullDevice()
{
	int fd = open("/dev/null",O_RDWR);
	printf("Testiranje: /dev/null fd: %d \n",fd);
	int size = 20;
	char buffer[20];
	

	memset(buffer,1, size);
	int r = read(fd,buffer,size);
	if(r != size || !checkBytes(buffer,1,size))
		printf("/dev/null read test failed\n");
	else
		printf("/dev/null read test succeeded\n");

	memset(buffer,1, size);
	r = write(fd,buffer,size);
	if(r != size || !checkBytes(buffer,1,size) )
		printf("/dev/null write test failed | size is %d \n",r);
	else
		printf("/dev/null write test succeeded\n");
	
}
void TestZeroDevice()
{
	int fd = open("/dev/zero",O_RDWR);
	printf("Testiranje: /dev/zero fd: %d \n",fd);
	int size = 20;
	char buffer[20];

	memset(buffer,1, size);
	int r = read(fd,buffer,size);
	if(r != size || !checkBytes(buffer,0,size))
		printf("/dev/zero read test failed| size is %d \n",r);
	else
		printf("/dev/zero read test succeeded\n");

	memset(buffer,1, size);
	r = write(fd,buffer,size);
	if(r != size || !checkBytes(buffer,1,size) )
		printf("/dev/zero write test failed | size is %d \n",r);
	else
		printf("/dev/zero write test succeeded\n");
	
}
void TestDevices()
{
	TestZeroDevice();
	TestNullDevice();
	
}



int
main(int argc, char *argv[])
{
	printf("Started testing\n\n");

	for(int i = 1; i < argc; i++)
	{
		if(!strcmp("console", argv[i]))
			TestConsoleSystemCalls();
		else if(!strcmp("dev", argv[i]))
			TestDevices();
	}

	printf("Testing completed\n");
	exit();
}
