#include <dlfcn.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct	/* needs to be binary-compatible with old versions */
{
#ifdef _STDIO_REVERSE
	unsigned char	*_ptr;	/* next character from/to here in buffer */
	int		_cnt;	/* number of available characters in buffer */
#else
	int		_cnt;	/* number of available characters in buffer */
	unsigned char	*_ptr;	/* next character from/to here in buffer */
#endif
	unsigned char	*_base;	/* the buffer */
	unsigned char	_flag;	/* the state of the stream */
	unsigned char	_file;	/* UNIX System file descriptor */
} FILE;

int fgetc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int getc(FILE *stream);
int getchar(void);
int ungetc(int c, FILE *stream);
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int dprintf(int fd, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int fclose(FILE *stream);



#define MAX_PATH 4096+1
#define LIBC_PATH "/lib/x86_64-linux-gnu/libc.so.6"

char cwd[MAX_PATH];

static int isInList(const char * path, const char * list, const char * file) {
	
	// load libc open to open list
	void * handle = dlopen(LIBC_PATH, RTLD_LAZY);
	FILE * (*libc_fopen)(const char *s, const char * mode) = (FILE * (*)(const char *, const char *)) dlsym(handle, "fopen");

	char buffer[strlen(path) + strlen(list) + 2];
	sprintf((char *)buffer, "%s/%s", path, list);

	// open list
	FILE * fp = libc_fopen( (char *)buffer, "r");
	if (fp == NULL) {
		return 0;
	}
	
	// interprete list
	int discard = 0;
	while(fgets(buffer, MAX_PATH, fp) != NULL) {
		// discard too long lines
		if (buffer[strlen(buffer) -1] != '\n') {
			discard = 1;
			continue;
		}
		// discard rest of too long line as well
		if (discard) {
			discard = 0;
			continue;
		}
		// delete new line character
		buffer[strlen(buffer) -1] = 0;
		// check if file is the searched one
		if (strcmp(file, (char *)buffer) == 0)
			return 1;
	}

	// close list
	fclose(fp);
	
	return 0;
}

static int hasPermission(const char *file) {

	// determine the real path of the file
	char real_file[MAX_PATH];
	if (!realpath(file ,real_file)) {
		return 0;
	}

	// determine the path to the while-/blacklist
	char * path;
	path = cwd;

	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		return 0;
	}

    char * envvar = "SANDBOX_CONFIG";
    char * config = getenv(envvar);
    if(config) {
        path = config;
    }


	// deny if real file is in blacklist
	if (isInList(path, "blacklist", real_file)) {
		return 0;
	}

	// permit if real_file is in whitelist
	if (isInList(path, "whitelist", real_file)) {
		return 1;
	}

	// ask if file is not in any list
	printf("The app requests access to '%s'. do you want to grant it? [y/n]: ", real_file);
	char res = getchar();

	if (res == 'y' || res == 'Y') return 1;
	return 0;
}

int open(const char *file, int oflag) {
	if (!hasPermission(file)) {
		errno = EPERM;
		return -1;
	}

	void * handle = dlopen(LIBC_PATH, RTLD_LAZY);
	int (*orig)(const char *s, int flags) = (int (*)(const char *, int)) dlsym(handle, "open");

	return orig(file, oflag);
}

int creat(const char *file, mode_t mode) {
	if (!hasPermission(file)) {
		errno = EPERM;
		return -1;
	}

	void * handle = dlopen(LIBC_PATH, RTLD_LAZY);
	int (*orig)(const char *s, int flags) = (int (*)(const char *, int)) dlsym(handle, "creat");

	return orig(file, mode);
}

int openat(int dirfd, const char *file, int flags) {

	if (fchdir(dirfd) == -1) {
		errno = EPERM;
		return -1;
	}
	if (!hasPermission(file)) {
		errno = EPERM;
		return -1;
	}
	chdir(cwd);

	void * handle = dlopen(LIBC_PATH, RTLD_LAZY);
	int (*orig)(int, const char *, int) = (int (*)(int, const char *, int)) dlsym(handle, "openat");

	return orig(dirfd, file, flags);
}

FILE * fopen(const char *file, const char *mode) {

	if (!hasPermission(file)) {
		errno = EPERM;
		return NULL;
	}

	void * handle = dlopen(LIBC_PATH, RTLD_LAZY);
	FILE* (*orig)(const char *, const char *) = (FILE * (*)(const char *, const char *)) dlsym(handle, "fopen");

	return orig(file, mode);
}
