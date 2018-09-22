#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEFAULT_WIDTH   143
#define DEFAULT_HEIGHT  46

size_t get_file_size(int fd) {
    size_t size = 0;
    
    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    return size;
}

int hash(const int *buffer) {
    int ret = 0;
   
    for(unsigned i = 0; i < (sizeof(buffer)/sizeof(*buffer)); ++i) {
        ret += (int)buffer[i];
    }

    return ret;
}

int usage(const char *filename) {
    fprintf(stderr, "usage: %s <filepath> [-w <width>] [-h <height>] [-c <character>]\n", filename);
    return -1;
}

int main(int argc, char *argv[]) {
    int     fd;
    int     opt;
    int     *buffer = NULL;

    unsigned width = DEFAULT_WIDTH;
    unsigned height = DEFAULT_HEIGHT;
    char     charac[16] = "\u2588";

    size_t   offset = 0;
    size_t   filesize = 0;

    char        *filepath = NULL;
    extern char *optarg;

    while((opt = getopt(argc, argv, "?o:h:w:c:")) != -1) {
        switch(opt) {
            case 'o':
                filepath = optarg;
                break;
            case 'w':
                width = atoi(optarg);
                break;
            case 'h':
                height = atoi(optarg);
                break;
            case 'c':
                strcpy(charac, optarg);
                break;
            case '?':
                return usage(argv[0]);
                break;
        }
    }

    fd = open(filepath, O_RDONLY); 

    if(fd < 1)  return 1;

    filesize = get_file_size(fd);
    offset = filesize / (width * height);

    buffer = malloc((offset + 1) * sizeof(int));
    if(!buffer) return 2;

    unsigned x = 0;
    unsigned y = 0;

    while(read(fd, buffer, offset) >= 0) {
        int color = hash(buffer) % 8;

        if(color < 0) color = 0;

        printf("\x1b[%dm%s", 30+color, charac);

        ++x;
        if(x >= width) {
            x = 0;
            ++y;
        
            if(y >= height) break;

            putchar('\n');
        }
    }
    printf("\x1b[0m \n");

    free(buffer);
    close(fd);

    return EXIT_SUCCESS;
}
