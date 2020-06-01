#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/file.h"


void writePreLogin()
{
	int fdIssue;
	if((fdIssue = open("/etc/issue", 0)) < 0)
		return;
	char buf[512];
	int n;
	while((n = read(fdIssue, buf, sizeof(buf))) > 0) {
		if (write(1, buf, n) != n) {
			fprintf(2, "getty: write error /etc/issue\n");
			exit();
		}
	}
}

void readLine(char* buffer, int n)
{
	char c;
	for(int i = 0; i < n; i++)
	{
		read(0,&c,1);
		if(c == '\n')
			break;
		buffer[i] = c;
	}
}
void readString(char* buffer, int n)
{
	char c;
	for(int i = 0; i < n; i++)
	{
		read(0,&c,1);
		if(c == '\n' || c == ' ')
			break;
		buffer[i] = c;
	}
}


void login()
{
	char username[512];
	char password[512];
	struct user u;
	while (1)
	{
		printf("Korisnicko ime: ");
		memset(username, 0, sizeof(username));
		readLine(username, sizeof(username));

		printf("Sifra: ");
		memset(password, 0, sizeof(password));
		setecho(0);
		readLine(password, sizeof(password));
		setecho(1);
		printf("\n");
		if(loginUser(&u,username, password))
			break;
		//printf("Nevalidni kredencijali\n", password);
	}
	
	
	
	
}

void writePostLogin()
{
	int fdIssue;
	if((fdIssue = open("/etc/motd", 0)) < 0)
		return;
	char buf[512];
	int n;
	while((n = read(fdIssue, buf, sizeof(buf))) > 0) {
		if (write(1, buf, n) != n) {
			fprintf(2, "getty: write error /etc/motd\n");
			exit();
		}
	}
}

void getty()
{
	writePreLogin();
	login();
	writePostLogin();
}

int
main(int argc, char *argv[])
{
	getty();
	exit();
}
