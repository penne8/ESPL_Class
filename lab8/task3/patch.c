#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <elf.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){

    int src_fd, dest_fd;
    char *src, *dest;


    if (argc != 6)
    {
        fprintf(stderr, "usage: ./patch source_file source_pos size dest_file dest_pos");
        return 1;
    }

    if ((src_fd = open(argv[1], O_RDONLY)) < 0)
    {
        fprintf(stderr, "source_file open failed");
        return 1;
    }
    size_t src_pos = (int)strtol(argv[2], NULL, 16);
    size_t copy_size = atoi(argv[3]);
    if ((dest_fd = open(argv[4], O_RDWR, 0666)) < 0)
    {
        fprintf(stderr, "dest_file open failed");
        return 1;
    }
    size_t dest_pos = (int)strtol(argv[5], NULL, 16);

    size_t src_file_size = lseek(src_fd, 0, SEEK_END);
    size_t des_file_size = lseek(dest_fd, 0, SEEK_END);
    
    src = mmap(NULL, src_file_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    dest = mmap(NULL, des_file_size, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);

    memcpy(dest+dest_pos, src+src_pos, copy_size);

    munmap(src, src_file_size);
    munmap(dest, des_file_size);

    close(src_fd);
    close(dest_fd);

    return 0;
}
