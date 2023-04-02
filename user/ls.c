#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
	static char buf[DIRSIZ+1];
	char *p;

	// Find first character after last slash.
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
		;
	p++;

	// Return blank-padded name.
	if(strlen(p) >= DIRSIZ)
		return p;
	memmove(buf, p, strlen(p));
	memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
	return buf;
}

void modestr(char* buffer, int mode, int directory)
{
	if(directory)
		buffer[0] = 'd';
	else
		buffer[0] = '-';
	for(int i = 2; i >= 0; i--)
	{
		int bits = mode & 0x7; //111
		buffer[1 + i * 3 + 0] = (bits & 0x4) ? 'r':'-';
		buffer[1 + i * 3 + 1] = (bits & 0x2) ? 'w':'-';
		buffer[1 + i * 3 + 2] = (bits & 0x1) ? 'x':'-';
		mode = mode >> 3;
	}
	buffer[10] =0;
}

void
ls(char *path)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;
	if((fd = open(path, 0)) < 0){

		fprintf(2, "ls: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "ls: cannot stat %s\n", path);
		close(fd);
		return;
	}

	char modebuff[11];
	struct user u;
	switch(st.type){
	case T_FILE:
		modestr(modebuff, st.mod, 0);
		getUser(&u,st.uid);
		printf("%s %s %s %d %d %d\n",modebuff,u.username, fmtname(path), st.type, st.ino, st.size);
		break;

	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
			printf("ls: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf("ls: cannot stat %s\n", buf);
				continue;
			}
			modestr(modebuff, st.mod, st.type == T_DIR);
			getUser(&u,st.uid);
			printf("%s %s %s %d %d %d\n",modebuff,u.username, fmtname(buf), st.type, st.ino, st.size);
		}
		break;
	}
	close(fd);
}

int
main(int argc, char *argv[])
{
	int i;

	if(argc < 2){
		ls(".");
		exit();
	}
	for(i=1; i<argc; i++)
		ls(argv[i]);
	exit();
}
