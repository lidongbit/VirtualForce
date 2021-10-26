#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/shm.h>
#define _GNU_SOURCE
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <linux/types.h>
#include <asm/byteorder.h>
#include <linux/ip.h>
#include <signal.h>

#include "network_client.h"
#include "force_interface.h"
#include "controller.h"
#define MIN(a,b) (a>b?b:a)

#define  CLIENT_BUFF_SIZE sizeof(Pc_Comm_Cmd_t)

typedef int int32_t;

static Net_Circle_Buff_t net_buff;

//extern int pipe_fd[2];

static int read_data(Net_Circle_Buff_t *net_ptr,int len);
static void network_client_connect(void);

void network_init(void )
{
    network_client_connect();
    net_buff.local_buff_info.head_index_offset = 0;
    net_buff.local_buff_info.tail_index_offset = 0;
    net_buff.local_buff_info.element_length = CLIENT_BUFF_SIZE;
    net_buff.local_buff_info.buff_length = MAX_SIZE;
    net_buff.local_buff_info.semaphore = 1;

    pthread_mutex_init(&net_buff.mutex, NULL);
    pthread_cond_init(&net_buff.notfull, NULL);
    pthread_cond_init(&net_buff.notempty, NULL);
    printf("network init!\n");
}
#define IPSIZE 1024
void network_client_connect(void)
{
    char ip[IPSIZE] = {0};
    net_buff.sfd = socket(AF_INET, SOCK_STREAM, 0);//ipv4 tcp
    if( net_buff.fd == -1)
    {
        perror("socket failed");
        exit(0);
    }

    int val = 1;
    if(setsockopt(net_buff.sfd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val)) < 0){
        perror("setsockopt()");
        exit(1);
    }
    // IPv4的地址结构体，并赋值为服务端的地址
    struct sockaddr_in srvaddr,raddr;
    bzero(&(srvaddr), sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", (void *)&(srvaddr.sin_addr));
    srvaddr.sin_port = htons(atoi("8811"));

    if(bind(net_buff.sfd,(void *)&srvaddr,sizeof(srvaddr)) < 0){
        perror("bind()");
        exit(1);
    }

    if(listen(net_buff.sfd,1024) < 0){//全连接数量
        perror("listen()");
        exit(1);
    }

    socklen_t raddr_len = sizeof(raddr);
    net_buff.fd = accept(net_buff.sfd,(void *)&raddr,&raddr_len);//接收客户端连接
    if (net_buff.fd < 0){
        perror("accept()");
        exit(1);
    }

    inet_ntop(AF_INET,&raddr.sin_addr,ip,IPSIZE);
    printf("client %s %d\n",ip,ntohs(raddr.sin_port));

    net_buff.fp = fdopen(net_buff.fd,"r+");
    if (net_buff.fp == NULL){
        perror("fopen()");
        exit(1);
    }
#if 0
    char buff[1024] = {0};
    *((int *)buff) = RES_INFO;
    strcpy(buff+4,"client online!\n");
    network_set(buff,1024);
#endif
}
void network_close(void *p)
{
    pthread_mutex_destroy(&net_buff.mutex);
    pthread_cond_destroy(&net_buff.notfull);
    pthread_cond_destroy(&net_buff.notempty);
    close(net_buff.fd);
}

void network_get(Net_Circle_Buff_t **net_ptr)
{
    *net_ptr =  &net_buff;
}

pid_t network_rcv(pid_t child)
{
    static int count = 0;
	static pid_t pid = 0;
    struct tcp_info info;
    int len=sizeof(info);
	char ip[IPSIZE] = {0};
	pid = child;
    getsockopt(net_buff.fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    //printf("info.tcpi_state:%d\n",info.tcpi_state);
    if(info.tcpi_state==1)
    {
        if(is_buff_full(&net_buff.local_buff_info))
        {
            printf("buff is full!\n");
            return pid;
        }
        read_data(&net_buff,CLIENT_BUFF_SIZE);

    }else{
		printf("disconnect!\n");
		kill(pid,SIGKILL);
		struct sockaddr_in raddr;
		socklen_t raddr_len = sizeof(raddr);
        close(net_buff.fd);
		
		net_buff.fd = accept(net_buff.sfd,(void *)&raddr,&raddr_len);//接收客户端连接
	    if (net_buff.fd < 0){
	        perror("accept()");
	    }	
		inet_ntop(AF_INET,&raddr.sin_addr,ip,IPSIZE);
	    printf("client %s %d\n",ip,ntohs(raddr.sin_port));
		
	    net_buff.fp = fdopen(net_buff.fd,"r+");
		
	    if (net_buff.fp == NULL){
	        perror("parent pid fopen()");
	        exit(1);
    	}
		pid = fork();
		if(pid==0)
		{
			printf("child pid: %d\n", getpid());
			while(1)
			{
				controller_display_res();
				usleep(200000);
			}
		}	
    }

    return pid;

}

void *network_producer( void *arg )
{
    static int count = 0;
    //nice(10);
    pthread_cleanup_push(network_close,NULL);
    while(1)
    {
        struct tcp_info info;
        int len=sizeof(info);
        getsockopt(net_buff.fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
        //printf("info.tcpi_state:%d\n",info.tcpi_state);
        if(info.tcpi_state==1)
        {
            pthread_mutex_lock(&net_buff.mutex);  // 为保证条件变量不会因为多线程混乱，所以先加锁
            if(is_buff_full(&net_buff.local_buff_info))
            {
                printf("buff is full!\n");
                pthread_mutex_unlock(&net_buff.mutex);
                pthread_yield();
                continue;
            }
            read_data(&net_buff,CLIENT_BUFF_SIZE);
            pthread_mutex_unlock(&net_buff.mutex);
            pthread_yield();

        }else{
            printf("disconnect!\n");
            // how to reconnect
        }
    }
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}


int read_data(Net_Circle_Buff_t *net_ptr,int len)
{
    int total_len = len;
    int read_len = 0;
    int rtn_len = 0;
    char buffer[CLIENT_BUFF_SIZE] = {0};
    while(total_len)
    {
        read_len = MIN(total_len, CLIENT_BUFF_SIZE);
        rtn_len = fread(buffer, sizeof(char), read_len, net_ptr->fp);
        if(rtn_len < read_len)  /* 读到数据小于预期 */
        {
            if(ferror(net_ptr->fp))
            {
                if(errno == EINTR) /* 信号使读操作中断 */
                {
                    /* 不做处理继续往下走 */;
                }
                else if(errno == EAGAIN || errno == EWOULDBLOCK) /* 发生了超时 */
                {
                    total_len -= rtn_len;
                    break;
                }
                else    /* 其他错误 */
                {
                    exit(1);
                }
            }
            else    /* 读到文件尾 */
            {
                total_len -= rtn_len;
                break;
            }
        }
        total_len -= rtn_len;
    }

    if(total_len != 0)
    {
        return -1;
    }else{
        printf("push! cmd :%d,\n", *((int *)buffer));
        push_circle_buff_item(&net_ptr->local_buff_info,net_ptr->local_buff,buffer);
        return 0;
    }
}

int network_set(char *buff, int length)
{
    int res = 0;
	struct tcp_info info;
    int len=sizeof(info);
	int tmp_fd = 0;
    getsockopt(net_buff.fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
 
    if(info.tcpi_state==1)
    {
		res = fwrite(buff,sizeof(char),length,net_buff.fp);
		if(res<0)
	    {
	        printf("socket send failed!\n");
	    }else{
	         fflush(net_buff.fp);
	    }
    }else{
 
		close(net_buff.fd);
	    exit(1);

    }
	
    return 0;
}
