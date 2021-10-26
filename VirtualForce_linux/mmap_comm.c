#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h> //mmap head file
#include <unistd.h>
#include <sys/shm.h>
#include "force_interface.h"
#include "mmap_comm.h"
#include <errno.h>

static MMAP_COMM_PTR_t mmap_addr;

void mmap_init(void)
{
    printf("mmap init start!\n");
    int fd = open("/dev/mem",O_RDWR|O_SYNC);

    if(fd<0)
    {
        printf("open /dev/mem failed!\n");
        exit(0);
    }

    printf("before mmap!\n");


    mmap_addr.cmd_ptr = mmap( NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SERVO_CMD_ADDR);
    if(mmap_addr.cmd_ptr == (void *)-1)
    {
        printf("mmap 2 failure!\n");
        exit(0);
    }

    mmap_addr.monitor_ptr = mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,FORCE_MONITOR);
    if(mmap_addr.monitor_ptr == (void *)-1)
    {
        printf("mmap monitor_ptr failure!\n");
        munmap(mmap_addr.cmd_ptr,4096);
        exit(0);
    }

    mmap_addr.virtual_ptr = mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,FORCE_GIVEN_VIR);
    if(mmap_addr.virtual_ptr == (void *)-1)
    {
        printf("mmap monitor_ptr failure!\n");
        munmap(mmap_addr.cmd_ptr,4096);
        munmap(mmap_addr.monitor_ptr,4096);
        exit(0);
    }

}

void mmap_get_comm_ptr(MMAP_COMM_PTR_t **ptr)
{
    *ptr = &mmap_addr;
}

void mmap_close(void)
{
    munmap( mmap_addr.cmd_ptr, 4096);
    munmap( mmap_addr.monitor_ptr, 4096);
    munmap( mmap_addr.virtual_ptr, 4096);
    printf( "umap ok \n" );
}
