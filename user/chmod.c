#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"


void changemode2(int group, int action, int type, char* files[],int n)
{
	for(int i =0 ; i < n; i++)
	{
		int fd, mode;
		struct stat st;
		if((fd = open(files[i], 0)) < 0){

			fprintf(2, "chmod: cannot open %s\n", files[i]);
			return;
		}

		if(fstat(fd, &st) < 0){
			fprintf(2, "chmod: cannot stat %s\n", files[i]);
			close(fd);
			return;
		}
		mode = st.mod;
		close(fd);
		if(action == 0) //+
		{
			if(group == 3)
			{
				mode = mode | (1 << type);
				mode = mode | ((1 << type)<<3);
				mode = mode | ((1 << type)<<6);
			}
			else
			{
				if(type == 3)
					mode = mode | (1 << 9);
				else
					mode = mode | ((1 << type) << group * 3);
			}
		}
		else //-
		{
			if(group == 3)
			{
				mode = mode & ~(1 << type);
				mode = mode & ~((1 << type)<<3);
				mode = mode & ~((1 << type)<<6);
			}
			else
			{
				if(type == 3)
					mode = mode & ~(1 << 9);
				else
					mode = mode & ~((1 << type) << group * 3);
			}
		}
		if(chmod(files[i], mode))
		{
			printf("Greska pri menjanju moda u fajlu %s\n", files[i]);
			return;
		}
		
	}
}

void changemode1(int mode, char* files[],int n)
{
	for(int i =0 ; i < n; i++)
	{
		if(chmod(files[i], mode))
		{
			printf("Greska pri menjanju moda u fajlu %s\n", files[i]);
			return;
		}
	}
}
int
atoo(const char *s)
{
	int n;
	n = 0;
	int i =0;
	while (1)
	{
		if(*s == '\n' || *s == 0 || *s == ' ')
			if(i == 0)
				return -1;
			else
				break;
		i++;
		if('0' <= *s && *s <= '7')
		{
			n = n << 3;
			n = n + (*s -  '0');
			s++;
			continue;
		}
		return -1;
	}
	return n;
}

int parseMod(char * arg, int* group, int* action, int* type)
{
	if(strlen(arg) != 3)
		return 0;
	switch (arg[0])
	{
	case 'u':
		*group = 2;
		break;
	case 'g':
		*group = 1;
		break;
	case 'o':
		*group = 0;
		break;
	case 'a':
		*group = 3;
		break;
	default:
		return 0;
	}
	switch (arg[1])
	{
	case '+':
		*action = 0;
		break;
	case '-':
		*action = 1;
		break;
	default:
		return 0;
	}
	switch (arg[2])
	{
	case 'r':
		*type = 2;
		break;
	case 'w':
		*type = 1;
		break;
	case 'x':
		*type = 0;
		break;
	case 's':
		*type = 3;
		break;
	default:
		return 0;
	}
	return 1;
}

int
main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Unesite fajlove!\n");
		return;
	}
	int mode;
	int group, action, type;
	if((mode = atoo(argv[1]) >= 0))
	{
		changemode1(mode, argv + 2, argc -2);
	}
	else if(parseMod(argv[1],&group, &action, &type))
	{
		changemode2(group,action,type, argv + 2, argc -2);
	}
	else
	{
		printf("Ne validan format");
		exit();
	}
	exit();
}
