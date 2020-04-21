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

void TestCler()
{
	clrscr();
	printf("Ociscen prozor\n");
}
void TestPos()
{
	int x;
	int y;
	getcp(&x,&y);
	printf("Trenutna pozicija kursora %d, %d\n",x,y);

	setcp(0,10);
	printf("Kursor y setovan na 10 a x na 0\n");
	setcp(15,12);
	printf("Kursor y setovan na 12 a x na 15\n");
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


void TestKMESG()
{
	int fd = open("/dev/kmesg",O_RDONLY);
	printf("Testiranje: /dev/kmesg fd: %d \n",fd);
	int size = 20;
	char buffer[20];

	memset(buffer,1, size);
	int r = read(fd,buffer,size);

	printf("KMESG prvih 15 karaktera: (ocekivano da sadrzi \"Sistem je pokrenut\" ako je prvi put izvrseno)\n",r);
	for(int i = 0; i < size;i++)
		if(buffer[i] != '0' && buffer[i] != '\n')
			printf("%c",buffer[i]);
	read(fd,buffer,size);
	printf("\nKMESG sledecih 15 karaktera: (ocekivano da sadrzi \"CPU starting\" ako je prvi put izvrseno)\n",r);
	for(int i = 0; i < size;i++)
		if(buffer[i] != '0' && buffer[i] != '\n')
			printf("%c",buffer[i]);
	printf("\n");
}

void TestRANDOm()
{
	int fd = open("/dev/random",O_RDONLY);
	printf("Testiranje: /dev/random fd: %d \n",fd);
	int size = 20;
	char buffer[20];

	memset(buffer,1, size);
	int r = read(fd,buffer,size);

	printf("RANDOM prvih 20 brojeva:\n",r);
	for(int i = 0; i < size;i++)
		printf("%d ",buffer[i]);
		r = read(fd,buffer,size);
	printf("\n");
	printf("RANDOM sledecih 20 brojeva:\n",r);
	for(int i = 0; i < size;i++)
		printf("%d ",buffer[i]);
	printf("\n");
	
}

void TestDevices()
{
	TestZeroDevice();
	TestNullDevice();
	TestKMESG();
	TestRANDOm();

}



void TestDD1()
{
	char *args[] = { "/bin/dd" , "if=/home/README","of=/home/readme2", 0 };
	exec("/bin/dd", args);
	exit();
}

void TestDD2()
{
	char *args[] = { "/bin/dd" , "if=/home/README"
	,"of=/home/snippet","skip=2","count=1", 0 };
	exec("/bin/dd", args);
	exit();
}

void TestDD3()
{
	char *args[] = { "/bin/dd" , "if=/dev/random"
	,"of=/home/keyfile","bs=64","count=2", 0 };
	exec("/bin/dd", args);
	exit();
}
void TestDD4()
{
	char *args[] = { "/bin/dd" , "if=/dev/random"
	,"of=/test","bs=8","count=1", "seek=7", 0 };
	exec("/bin/dd", args);
	exit();
}
void TestDD5()
{
	char *args[] = { "/bin/dd" , "if=/dev/random", "count=1", 0 };
	exec("/bin/dd", args);
	exit();
}


void TestSeek()
{
	int file = open("/home/seekTest", O_CREATE | O_RDWR);
	int console = open("/dev/console", O_WRONLY);


	char buff[50];
	int i;
	memset(buff, 'A',50);
	for(i = 40; i< 60;i++)
		buff[i -40] = i;
	lseek(file, 10, 0);
	write(file,buff,20);

	lseek(file,0,0);
	memset(buff, 'C',50);
	write(file, buff,3);
	lseek(file,10,1);
	write(file, buff,3);
	memset(buff, 'D',50);
	//lseek(file,0,0);
	lseek(file,10,2);
	write(file, buff,5);

	memset(buff, 'E',50);

	lseek(file,0,0);
	read(file,buff,50);
	write(console,buff,50);
	printf("\n");
}


void TestDevSeek()
{
	int file = open("/dev/kmesg", O_RDWR);
	int disk = open("/dev/disk", O_RDWR);
	int console = open("/dev/console", O_WRONLY);


	char buff[100];
	lseek(file,10,0);
	read(file,buff,10);
	lseek(file, 24, 1);
	read(file,buff +10,40);
	
	lseek(disk, 100, 0);
	read(disk,buff +50,40);
	lseek(disk, 150, 1);
	read(disk,buff +90,10);

	write(console,buff,100);
	printf("\n");
}
void TestDevDisk()
{
	char *args[] = { "/bin/dd" , "if=/dev/zero", "of=/dev/disk", 0 };
	exec("/bin/dd", args);
	exit();
}




int
main(int argc, char *argv[])
{
	printf("Started testing\n\n");

	for(int i = 1; i < argc; i++)
	{
		if(!strcmp("console", argv[i]))
			TestConsoleSystemCalls();
		else if(!strcmp("clear", argv[i]))
			TestCler();
		else if(!strcmp("pos", argv[i]))
			TestPos();
		else if(!strcmp("dev", argv[i]))
			TestDevices();
		else if(!strcmp("zero", argv[i]))
			TestZeroDevice();
		else if(!strcmp("null", argv[i]))
			TestNullDevice();
		else if(!strcmp("kmesg", argv[i]))
			TestKMESG();
		else if(!strcmp("random", argv[i]))
			TestRANDOm();	
		else if(!strcmp("dd1", argv[i]))
			TestDD1();	
		else if(!strcmp("dd2", argv[i]))
			TestDD2();	
		else if(!strcmp("dd3", argv[i]))
			TestDD3();	
		else if(!strcmp("dd4", argv[i]))
			TestDD4();	
		else if(!strcmp("dd5", argv[i]))
			TestDD5();
		else if(!strcmp("seek", argv[i]))	
			TestSeek();
		else if(!strcmp("devseek", argv[i]))	
			TestDevSeek();
		else if(!strcmp("disk", argv[i]))	
			TestDevDisk();
	}

	printf("Testing completed\n");
	exit();
}
