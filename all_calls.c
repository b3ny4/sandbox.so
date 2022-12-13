#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 1000
char buffer[BUFFER_SIZE];

int main(int argc, char *argv[]) {

    // open
    int file = open(argv[1], O_RDONLY);
    if (file == -1) {
        perror("open");
    } else {
        printf("Vulnerability found in open(const char * pathname, int mode)\n");
    }

    file = open(argv[1], O_RDONLY, 644);
    if (file == -1) {
        perror("open");
    } else {
        printf("Vulnerability found in open(const char * pathname, int mode, mode_t mode)\n");
    }

    // creat
    file = creat(argv[1], 644);
    if (file == -1) {
        perror("creat");
    } else {
        printf("Vulnerability found in creat(const char * pathname, mode_t mode)\n");
    }



    // openat
    int dir = open(".", O_DIRECTORY);
    if (dir == -1) {
        perror("open");
    } else {
        file = openat(dir, argv[1], O_RDONLY);
        if (file == -1) {
            perror("openat");
        } else {
            printf("Vulnerability found in openat(int dirfd, const char * pathname, int flags)\n");
        }

        file = openat(dir, argv[1], O_RDONLY, 644);
        if (file == -1) {
            perror("openat");
        } else {
            printf("Vulnerability found in openat(int dirfd, const char * pathname, int flags, mode_t mode)\n");
        }
    }

    // fopen
    FILE * fd = fopen(argv[1], "r");
    if (fd == NULL) {
        perror("fopen");
    } else {
        printf("Vulnerability found in fopen(const char * pathname, const char * mode)\n");
    }

    return 0;

}