#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/mman.h>           //mmap head file
#include <unistd.h>
#include <sys/shm.h>
#include <signal.h>
#include "force_interface.h"
#include "mmap_comm.h"          //comm with force core
#include "shmem_client.h"       //comm with server
#include "controller.h"
#include "network_client.h"

static void cleanup_handler(void);

static void cleanup_handler(void)
{
    //shmem_close();
    mmap_close();
}

#if 0
int main()
{
#if 1
    pthread_attr_t attr={0};
    struct sched_param sched = {0};
    sched.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &sched);
#endif
    printf("force client init start!\n");
    mmap_init();
    //shmem_init();
    network_init();
    //Servo_Force_Comm_Init();
    atexit(cleanup_handler);

    printf("force client init ok!\n");
    printf("SCHED_FIFO: %d %d\n",sched_get_priority_max(SCHED_FIFO),sched_get_priority_min(SCHED_FIFO));
    pthread_t tid_comm,tid_rcv,tid_up2pc;

    pthread_create(&tid_rcv,NULL,network_producer,NULL);
    pthread_create(&tid_comm,NULL,controller_msg_process,NULL);
    pthread_create(&tid_up2pc,&attr,controller_display_result,NULL);

    pthread_join(tid_rcv,NULL);
    pthread_join(tid_comm,NULL);
    pthread_join(tid_up2pc,NULL);
    return 0;
}
#else
static pthread_t tid_rcv,tid_send;
static pid_t pid_child;

void sighandler(int signum)
{
   printf("pid: %d 捕获信号 %d，跳出...\n", getpid(),signum);
   //pthread_cancel(tid_send);
   kill(pid_child,SIGKILL);
   mmap_close();
   exit(1);
}

int main()
{
    mmap_init();
    network_init();
    Servo_Force_Comm_Init();    
	
    pid_child = fork();
	if(pid_child==0)
	{
		printf("child pid: %d\n", getpid());
		while(1)
		{
			controller_display_res();
			usleep(200000);
		}
	}else if(pid_child>0){
		signal(SIGINT, sighandler);
		signal(SIGCHLD, SIG_IGN);
		printf("parent pid: %d\n", getpid());
		while(1)
		{
			pid_child = network_rcv(pid_child);
			controller_msg_parase();
			usleep(100000);
		}

	}
    return 0;
}
#endif
