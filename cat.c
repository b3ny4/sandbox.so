#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 1000
char buffer[BUFFER_SIZE];

int main(int argc, char *argv[]) {
    
    for(int idx = 1; idx < argc; idx++) {
        // int open(const char *pathname, int flags);
        int file = open(argv[idx], O_RDONLY);
        if (file == -1) {
            perror("open");
            continue;
        }
        
        ssize_t r = 0;
        while((r = read(file, buffer, BUFFER_SIZE-1)) > 0) {
            printf("%s", buffer);
            fflush(stdout);
        }
        if (r < 0) {
            perror("read");
        }

        if(close(file) == -1) {
            perror("close");
            continue;
        }
    }

    return 0;

}