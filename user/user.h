struct stat;
struct rtcdate;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int getuid();
int geteuid();
int setuid(int);
int setgroups(int,int*);
int chmod(const char*,int);
int chown(const char*,int,int);
int setecho(int);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
char* strncpy(char*, const char*, int);
char* safestrcpy(char*, const char*, int);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
int strstarts(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int readLine(char*, int);
int freadLine(int fd, char*, int);
int strSplit(char*, int, char**, int);
// auth.c

#define USERMAXLEN 15
#define PASSMAXLEN 30
#define RNAMEMAXLEN 30
#define USERPATHMAXLEN 100

#define GROUPNAMEMAXLEN 15
#define GROUPUSERMAXLEN 20

#define MAXUSERS 10
#define MAXGROUPS 10


struct user
{
    char username[USERMAXLEN];
    char password[PASSMAXLEN];
    int uid;
    int gid;
    char realName[RNAMEMAXLEN];
    char home[USERPATHMAXLEN];
};

struct group
{
    char name[USERMAXLEN];
    int gid;
    struct user users[GROUPUSERMAXLEN];
};

int getUser(struct user* buffer, int uid);
int getUserByName(struct user* buffer, char * name);
int loginUser(struct user* user, char * password);
int getGroup(struct group* buffer, int gid);
int getGroupByName(struct group* buffer, char * name);
int updateUser(struct user* user);
int addUser(struct user* user);
int removeUser(struct user* user);

int updateGroup(struct group* group);
int addGroup(struct group* group);
int removeGroup(struct group* group);

int getGroupsByUser(struct group* g , int* len, struct user* u);