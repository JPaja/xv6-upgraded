#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
//#include "kernel/file.h"
#define SEEK_SET 0

int
startsWith(const char *q,const char *p)
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

void dd(const char* inf, const char* of, int bs, int count, int skip, int seek)
{
	char buf[bs];

	int input = open(inf,O_RDONLY);
	int output = open(of,O_CREATE | O_WRONLY);
	lseek(input, skip * bs, SEEK_SET);
	lseek(output, seek * bs, SEEK_SET);
	int n;
	for(int i = 0; ((count < 0)? 1: (i < count));i++)
	{
		memset(buf,0,bs);
		n = read(input, buf, sizeof(buf));
		if(n == 0 && count < 0)
			break;
		write(output,buf, bs);
		
	}
	//printf("if=%s\nof=%s\nbs=%d\ncount=%d\nskip=%d\nseek=%d\n",inf,of,bs,count,skip,seek);
}


int
main(int argc, char *argv[])
{
	char buffer[512];
	int i;

	//args
	char inf[512]  = "/dev/console";
	char of[512] = "/dev/console";
	int bs = 512;
	int count = -1;
	int skip = 0;
	int seek = 0;
	for(i=1; i<argc; i++)
	{
		if(startsWith(argv[i],"if="))
			strcpy(inf, argv[i] + strlen("if="));
		else if(startsWith(argv[i],"of="))
			strcpy(of, argv[i] + strlen("of="));
		else if(startsWith(argv[i],"bs="))
		{
			strcpy(buffer, argv[i] + strlen("bs="));
			bs = atoi(buffer);
		}
		else if(startsWith(argv[i],"count=")){
			strcpy(buffer, argv[i] + strlen("count="));
			count = atoi(buffer);
		}
		else if(startsWith(argv[i],"skip=")){
			strcpy(buffer, argv[i] + strlen("skip="));
			skip = atoi(buffer);
		}
		else if(startsWith(argv[i],"seek=")){
			strcpy(buffer, argv[i] + strlen("seek="));
			seek = atoi(buffer);
		}
		else{
			printf("Error failed to parse argument: %s\n",argv[i]);
			return 0;
		}
	}
	if(count < 0)
		count = -1;
	dd(inf,of,bs,count,skip,seek);

	exit();
}
