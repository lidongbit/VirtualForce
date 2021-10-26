#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <pthread.h>
int pthread_yield(void);

#include "controller.h"
#include "force_interface.h"
#include "string.h"
#include "shmem_client.h"
#include "network_client.h"

typedef buffer_info_t SERVO_COMM_RINGS_BUFF_STRUCT;

#define BUFF_SIZE sizeof(Pc_Comm_Cmd_t)
static Net_Circle_Buff_t *net_ptr;

void handler(int num)
{
    int status;
    int pid = waitpid(-1, &status, WNOHANG);

    if (WIFEXITED(status)) {
        printf("The child %d exit with code %d\n", pid, WEXITSTATUS(status));}

}
static Pc_Comm_Cmd_t vforce_send;
void controller_msg_parase()
{
    static pthread_t send_tid;
    char buff[BUFF_SIZE] = {0};
    network_get(&net_ptr);
    int res = 0;
    Pc_Comm_Cmd_t *cmd = NULL;
    if(is_buff_empty(&(net_ptr->local_buff_info)))
    {
        return;
    }
    bzero(buff,BUFF_SIZE);
    //printf("before pull!\n");
    res = pull_circle_buff_item(&(net_ptr->local_buff_info),&(net_ptr->local_buff),buff);

    cmd = (Pc_Comm_Cmd_t*)buff;
    printf("receive cmd:%d param[6]:%d %d %d %d %d %d\n",
           cmd->cmd,cmd->params[0],cmd->params[1],cmd->params[2],cmd->params[3],cmd->params[4],cmd->params[5]);
#if 1
    if(res<0)
    {
        printf("pull_circle_buff_item error!\n");
    }else{

        switch(cmd->cmd)
        {
        case Cmd_resetFault:
            Servo_Force_Comm_Cmd_ResetFault();
            break;
        case Cmd_idle:
            Servo_Force_Comm_Cmd_Idle();
            break;
        case Cmd_working:
            Servo_Force_Comm_Cmd_Working();
            break;
        case Cmd_setFault:
            Servo_Force_Comm_Cmd_SetFault();
            break;
        case FORCE_GIVE_START:
            //memcpy(&vforce_send,cmd,sizeof(Pc_Comm_Cmd_t));
            //pthread_create(&send_tid, NULL, contoller_send_thread, NULL);
            Force_Virtual_Download(cmd);
            break;
        case FORCE_GIVE_STOP:
            //if(pthread_cancel(send_tid))
                //printf("pthread_cancel error!\n");
            Force_Virtual_Download(cmd);
            break;
        default:
            break;
        }
    }
#endif
    return;
}
void *contoller_send_thread(void *cmd)
{
    Pc_Comm_Cmd_t tmp;
    memcpy(&tmp, &vforce_send, sizeof(Pc_Comm_Cmd_t));
    printf("send thread: %d %d %d %d %d %d\n",tmp.params[0],tmp.params[1],tmp.params[2],tmp.params[3],tmp.params[4],tmp.params[5]);
    while(1)
    {
        Force_Virtual_Download(&tmp);
        usleep(2000);
    }
}
/*process network buffer msg*/
void *controller_msg_process(void *p)
{
    //pthread_detach(pthread_self());
    char buff[BUFF_SIZE] = {0};
    //shmem_get(&local_buff_info,&local_buff);
    network_get(&net_ptr);
    int res = 0;
    Pc_Comm_Cmd_t *cmd = NULL;
    //nice(10);
    while(1)
    {
        res = 0;
        pthread_mutex_lock(&(net_ptr->mutex));
        if(is_buff_empty(&(net_ptr->local_buff_info)))
        {
            //printf("buff empty!\n");
            pthread_mutex_unlock(&(net_ptr->mutex));
            pthread_yield();
            //usleep(1000);
            continue;
        }
        //printf("before pull!\n");
        res = pull_circle_buff_item(&(net_ptr->local_buff_info),&(net_ptr->local_buff),buff);
        cmd = (Pc_Comm_Cmd_t*)buff;
        printf("receive cmd:%d param[6]:%f %f %f %f %f %f\n",
               cmd->cmd,cmd->params[0],cmd->params[1],cmd->params[2],cmd->params[3],cmd->params[4],cmd->params[5]);
        if(res<0)
        {
            printf("pull_circle_buff_item error!\n");
        }
        pthread_mutex_unlock(&(net_ptr->mutex));
        //controller_parase_cmd(buff,BUFF_SIZE);
        bzero(buff, BUFF_SIZE);
    }

    pthread_exit(0);
}

void controller_display_res()
{
    FORCE_BACK_INFO_t res;
    char buff[sizeof(Pc_Comm_Data_t)];
    Pc_Comm_Data_t *pdata = NULL;
    int ret ;
    static int j = 0;

    bzero(buff,sizeof(Pc_Comm_Data_t));
    pdata = (Pc_Comm_Data_t*)buff;
    pdata->head = 0x11181118;
#if 1
    Force_Monitor_Upload(pdata);
#else
    pdata->force_state = 2;
    for(int i=0;i<6;i++)
    {
        pdata->force_theta[i] = 0.1118+j;
        j++;
    }
#endif
    network_set(buff,sizeof(Pc_Comm_Data_t));
    //printf("upload index: %d\n",j);

}
pthread_mutex_t gmutex;
pthread_cond_t gcond;
void *controller_display_result(void *p)
{
    FORCE_BACK_INFO_t res;
    char buff[sizeof(Pc_Comm_Data_t)];
    Pc_Comm_Data_t *pdata = NULL;
    static int j = 0;

    struct timeval now;
    struct timespec outtime;
    //struct timeval timeout;
#if 0
    pthread_mutex_init(&gmutex, NULL);
    pthread_cond_init(&gcond, NULL);
#endif
    int ret;
    //nice(-10);
    //setCurrentThreadHighPriority(1,0);
    while(1)
    {

        //pthread_mutex_lock(&gmutex);
#if 0
        gettimeofday(&now,NULL);
        outtime.tv_sec = 0;
        outtime.tv_nsec = 100000000;
#endif
        //ret = pthread_cond_timedwait(&gcond, &gmutex, &outtime);
        //if(ret!=0)
        {
            bzero(buff,sizeof(Pc_Comm_Data_t));
            pdata = (Pc_Comm_Data_t*)buff;
            pdata->head = 0x11181118;
            pdata->force_state = 0;

            for(int i=0; i<6; i++)
            {
                pdata->force_theta[i] = j+i;
            }
            j++;

            network_set(buff,sizeof(Pc_Comm_Data_t));
            printf("upload index: %d\n",j);
        }
        //nanosleep(&outtime,NULL);
        //pthread_mutex_unlock(&gmutex);
        usleep(100000);
    }

}

void setCurrentThreadHighPriority(int value, int p)
{
  // Start out with a standard, low-priority setup for the sched params.
  struct sched_param sp;
  bzero((void*)&sp, sizeof(sp));
  int policy = SCHED_OTHER;

  // If desired, set up high-priority sched params structure.
  if (value) {
    // FIFO scheduler, ranked above default SCHED_OTHER queue
    policy = SCHED_FIFO;
    // The priority only compares us to other SCHED_FIFO threads, so we
    // just pick a random priority halfway between min & max.
    const int priority = (sched_get_priority_max(policy) + sched_get_priority_min(policy)) / 2 + p;

    sp.sched_priority = priority;
  }

  // Actually set the sched params for the current thread.
  if (0 == pthread_setschedparam(pthread_self(), policy, &sp)) {
    printf("IO Thread #%d using high-priority scheduler!", pthread_self());
  }
}
