#include <string.h>
#include "force_interface.h"
#include "mmap_comm.h"
#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
static int *comm_info;
static ServoCoreProcessCall_t *service_info;
static PARA_FORCE_INFO_t *para_info;
static FORCE_INSTRUCTION_INFO_t *instruct_info;
static FORCE_CORE_STATE_t *state_info;
static FORCE_BACK_INFO_t *back_info;

static Force_Monitor_t *monitor_info;
static Force_Virtual_Cmd_t *virtual_info;

static MMAP_COMM_PTR_t *mmap_struct_ptr;

static void Servo_Force_Comm_Set_Para_UpdateFlag(int val);
static  void Servo_Force_Comm_Set_Cmd(ServoCoreProcessCall_t *app);
/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Init(void)
 * Param: none
 * Return: none
 * Brief: init force servo communicaiton interface
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_Init(void)
{
    mmap_get_comm_ptr(&mmap_struct_ptr);
    service_info = (ServoCoreProcessCall_t*)mmap_struct_ptr->cmd_ptr;
    comm_info = (int*)mmap_struct_ptr->comm_ptr;
    para_info = (PARA_FORCE_INFO_t*)mmap_struct_ptr->para_ptr;
    state_info = (FORCE_CORE_STATE_t*)mmap_struct_ptr->state_ptr;
    back_info = (FORCE_BACK_INFO_t*)mmap_struct_ptr->res_ptr;
    instruct_info = (FORCE_INSTRUCTION_INFO_t*)mmap_struct_ptr->ins_ptr;

    monitor_info = (Force_Monitor_t*)mmap_struct_ptr->monitor_ptr;
    virtual_info = (Force_Virtual_Cmd_t*)(mmap_struct_ptr->virtual_ptr);
    printf("Servo force comm init!\n");
}
/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Set_Para_UpdateFlag(int val)
 * Param: int[in] value of update flag
 * Return: none
 * Brief: set value of update flag
 * ----------------------------------------------------------------*/
int Servo_Force_Comm_Get_Para_UpdateFlag(void)
{
    return para_info->update_flag;
}
/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Set_Para_UpdateFlag(int val)
 * Param: int[in] value of update flag
 * Return: none
 * Brief: set value of update flag
 * ----------------------------------------------------------------*/
static void Servo_Force_Comm_Set_Para_UpdateFlag(int val)
{
    para_info->update_flag = val;
}

/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_ShakeHand_Request(void)
 * Param: void
 * Return: none
 * Brief: servo core send shank hand request
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_ShakeHand_Request(void)
{
    *comm_info = MAGIC_REQ_DATA;
}
/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Bind(void)
 * Param: void
 * Return: none
 * Brief: servo core bind communication
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_ShakeHand_Bind(void)
{
    while(*comm_info!=MAGIC_BIND_DATA)
        ;
    *(comm_info+3) = MAGIC_CONNECT_DATA;
}
/*-----------------------------------------------------------------
 * Function: int Servo_Force_Comm_Get_ForceHeart_(void)
 * Param: void
 * Return: int, servo
 * Brief: get force core heartbeat
 * ----------------------------------------------------------------*/
int Servo_Force_Comm_Get_ForceHeart(void)
{
    return *(comm_info+2) ;
}
/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_ServoHeart_Inc(void)
 * Param: void
 * Return: none
 * Brief: servo core heartbeat increase
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_ServoHeart_Inc(void)
{
    *(comm_info+2) = *(comm_info+2) + 1;
}
/*-----------------------------------------------------------------
 * Function: static void Servo_Force_Comm_Set_Cmd(ServoCoreProcessCall_t *app)
 * Param: ServoCoreProcessCall_t[in] *app
 * Return: none
 * Brief: copy local command to interface
 * ----------------------------------------------------------------*/
static void Servo_Force_Comm_Set_Cmd(ServoCoreProcessCall_t *app)
{
    //printf("before set cmd %d\n",app->cmd);
    memcpy((void*)service_info,app,sizeof(ServoCoreProcessCall_t));
    //printf("after set cmd %d\n",app->cmd);
}

/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Cmd_Working(void)
 * Param: void
 * Return: none
 * Brief: send working command
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_Cmd_Working(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_WORKING;
    Servo_Force_Comm_Set_Cmd(&app);
}
/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Cmd_Idle(void)
 * Param: void
 * Return: none
 * Brief: send idle command
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_Cmd_Idle(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_IDLE;
    Servo_Force_Comm_Set_Cmd(&app);
}

/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Cmd_Mode(int mode, int submode)
 * Param: int[in]
 * Param: int[in]
 * Return: none
 * Brief: set force core work mode and submode
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_Cmd_Mode(int mode, int submode)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_SWITCH_MODE;
    app.param1 = mode;
    app.param2 = submode;
    Servo_Force_Comm_Set_Cmd(&app);
}

void Servo_Force_Comm_Cmd_SetFault(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_SET_FAULT;
    Servo_Force_Comm_Set_Cmd(&app);
}
void Servo_Force_Comm_Cmd_ResetFault(void)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_RESET_FAULT;
    Servo_Force_Comm_Set_Cmd(&app);
}
/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Cmd_DignoseCfg(void)
 * Param: void
 * Return: none
 * Brief: config dignose data
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_Cmd_DignoseCfg(FORCE_DIGNOSE_VAR_e *ptr)
{
    ServoCoreProcessCall_t app={0};
    app.cmd = CMD_SWITCH_MODE;

    app.param1 = ptr[0] | (ptr[1]<<16);
    app.param2 = ptr[2] | (ptr[3]<<16);
    app.param3 = ptr[4] | (ptr[5]<<16);

    Servo_Force_Comm_Set_Cmd(&app);
}
/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Set_Para(PARA_READ_INFO_t *para_ptr)
 * Param: PARA_READ_INFO_t*[in]
 * Return: none
 * Brief: set parameters
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_Set_Para(PARA_FORCE_STD_INFO_t *para_ptr)
{
        int32_t *src = (int32_t*)(para_ptr->dh_d1);
        double *dst = (double*)(para_info->para_data);
        para_info->posture[0] = para_ptr->arm;
        para_info->posture[1] = para_ptr->elbow;
        para_info->posture[2] = para_ptr->wrist;
        para_info->posture[3] = para_ptr->flip;

        for(int i=0; i<(sizeof(PARA_FORCE_STD_INFO_t)-32)/sizeof(int32_t); i++)
        {
            *dst = ((double)(*(src+i)))/16777216.0;
        }
        Servo_Force_Comm_Set_Para_UpdateFlag(1);
}
/*-----------------------------------------------------------------
 * Function: int Servo_Force_Comm_Set_Instruct(SERVO_COMM_RINGS_BUFF_STRUCT *push_ptr,int32_t length)
 * Param: PARA_READ_INFO_t*[in]
 * Param: int32_t[in]
 * Return: int, -1 buffer full ; 0 success
 * Brief: set parameters
 * ----------------------------------------------------------------*/
int Servo_Force_Comm_Set_Instruct(FORCE_INSTRUCTION_INFO_t *push_ptr)
{
    if(instruct_info->valid==0)
    {
#if 1
        printf("in_pos_given: %lf %lf %lf %lf %lf %lf\n",
                    push_ptr->in_pos_given[0],push_ptr->in_pos_given[1],push_ptr->in_pos_given[2],
                    push_ptr->in_pos_given[3],push_ptr->in_pos_given[4],push_ptr->in_pos_given[5]);
        printf("in_force_back: %lf %lf %lf %lf %lf %lf\n",
                    push_ptr->in_force_back[0],push_ptr->in_force_back[1],push_ptr->in_force_back[2],
                    push_ptr->in_force_back[3],push_ptr->in_force_back[4],push_ptr->in_force_back[5]);
#endif
        memcpy(instruct_info,push_ptr,sizeof(FORCE_INSTRUCTION_INFO_t));
        instruct_info->valid = 1;
        return 0;
    }
    return -1;
}
/*-----------------------------------------------------------------
 * Function: int Servo_Force_Comm_Set_Instruct(SERVO_COMM_RINGS_BUFF_STRUCT *push_ptr,int32_t length)
 * Param: PARA_READ_INFO_t*[in]
 * Param: int32_t[in]
 * Return: int, -1 error ; 0 success
 * Brief: set parameters
 * ----------------------------------------------------------------*/
int Servo_Force_Comm_Get_Result(FORCE_BACK_INFO_t *pull_ptr)
{
    if(back_info->valid)
    {
        memcpy(pull_ptr,back_info,sizeof(FORCE_BACK_INFO_t));
        back_info->valid = 0;
        return 0;
    }
    return -1;
}
/*-----------------------------------------------------------------
 * Function: void Servo_Force_Comm_Discard_Result(void)
 * Param: void
 * Return: void
 * Brief: discard history result
 * ----------------------------------------------------------------*/
void Servo_Force_Comm_Discard_Result(void)
{
    //buffer_discard(circle_buff_res_info);
}

int Servo_Force_Comm_Get_State(void)
{
    return *(comm_info+4);
}

void Force_Monitor_Upload(Pc_Comm_Data_t *monitor)
{
    if(monitor_info->valid==1)
    {
        monitor->force_state = monitor_info->force_state;
        memcpy(&monitor->force_theta, &monitor_info->force_theta, 6*sizeof(double));
        monitor_info->valid = 0;
    }else{

    }
}
void Force_Virtual_Download(Pc_Comm_Cmd_t *vcmd)
{
    static int i=0;
#if 0
    int sum = 0;
    char *p = (char*)&vcmd->params[0];
    for(int j = 0; j < 6*sizeof(double); j++)
    {
       sum += p[j];
    }
#endif
    if(virtual_info->valid==0)
    {
        printf(" virtual force download: %d\n",i++);
        for(int j=0;j<6;j++)
        {
            virtual_info->force[j] = vcmd->params[j];
        }
        //virtual_info->sum = sum;

        virtual_info->valid = 0xdeadbeef;
    }
}
void Force_Virtual_Reset(void)
{
    memset(virtual_info->force,0,6*sizeof(int));
}
