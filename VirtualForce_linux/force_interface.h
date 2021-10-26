#ifndef __SERVO_FORCE_H__
#define __SERVO_FORCE_H__
#include "buffer_manager.h"

/*communication shake*/
#define COMM_SHAKE_HANDE_DATA (*((volatile unsigned int*)0x3b000000))
#define SERVO_CORE_HEART (*((volatile unsigned int*)0x3b000004))
#define FORCE_CORE_HEART (*((volatile unsigned int*)0x3b000008))
#define FORCE_CORE_HANDE_BAK (*((volatile unsigned int*)0x3b00000c))
#define FORCE_CORE_SM_STATE (*((volatile unsigned int*)0x3b000010))
/*parameters interface*/
//#define SERVO_PARA_ADDR  (0x3b002008)
//#define SERVO_PARA_UPDATE_FLAG (0x3b002000)
#define SERVO_PARA_ADDR  (0x3b002000)
/*command interface*/
#define SERVO_CMD_ADDR  (0x3b001000)
#define SERVO_CMD_BACK_ADDR (0x3b001020)

/*instruction feedback interface*/
#define SERVO_INSTRUCT_ADDR  (0x3b200000)
#define SERVO_STATE_ADDR (0x3b201000)
#define SERVO_RES_ADDR (0x3b202000)

/*force value*/
#define FORCE_MONITOR   (0x3b310000)
#define FORCE_GIVEN_VIR (0x3b320000)

/*const value*/
#define MAGIC_REQ_DATA 0xaa55aa55
#define MAGIC_BIND_DATA 0x55aa55aa
#define MAGIC_CONNECT_DATA 0xa5a5a5a5
/*参数需求表（上位《==》伺服核）*/
typedef struct
{
/*以下参数无转换*/
    //机器人形态配置参数
    int32_t arm;    // 1: right arm, -1:left arm
    int32_t elbow;  // 1: elbow above wrist, -1:elbow below wrist
    int32_t wrist;  // 1: wrist down, -1: wrist up
    int32_t flip;   // 0: not flip wrist, 1: flip wrist
/*以下参数Q24定标*/
    //机器人DH表
    int32_t dh_d1;
    int32_t dh_a1;
    int32_t dh_alpha1;
    int32_t dh_offset1;
    int32_t dh_d2;
    int32_t dh_a2;
    int32_t dh_alpha2;
    int32_t dh_offset2;
    int32_t dh_d3;
    int32_t dh_a4;
    int32_t dh_alpha4;
    int32_t dh_offset4;
    int32_t dh_d4;
    int32_t dh_a5;
    int32_t dh_alpha5;
    int32_t dh_offset5;
    int32_t dh_d6;
    int32_t dh_a6;
    int32_t dh_alpha6;
    int32_t dh_offset6;
    //坐标系0对基坐标系的姿态矩阵
    int32_t base_rmtx_0_0;
    int32_t base_rmtx_0_1;
    int32_t base_rmtx_0_2;
    int32_t base_rmtx_1_0;
    int32_t base_rmtx_1_1;
    int32_t base_rmtx_1_2;
    int32_t base_rmtx_2_0;
    int32_t base_rmtx_2_1;
    int32_t base_rmtx_2_2;
    int32_t base_p_x;
    int32_t base_p_y;
    int32_t base_p_z;
    //导纳参数Bd
    int32_t bd_v1;
    int32_t bd_v2;
    int32_t bd_v3;
    int32_t bd_v4;
    int32_t bd_v5;
    int32_t bd_v6;
    //导纳参数Kd
    int32_t kd_v1;
    int32_t kd_v2;
    int32_t kd_v3;
    int32_t kd_v4;
    int32_t kd_v5;
    int32_t kd_v6;
    //导纳参数Md
    int32_t md_v1;
    int32_t md_v2;
    int32_t md_v3;
    int32_t md_v4;
    int32_t md_v5;
    int32_t md_v6;
    //阻抗控制pid参数（预留）
    int32_t imped_kp_1;
    int32_t imped_ki_1;
    int32_t imped_kd_1;
    int32_t imped_kp_2;
    int32_t imped_ki_2;
    int32_t imped_kd_2;
    int32_t imped_kp_3;
    int32_t imped_ki_3;
    int32_t imped_kd_3;
    int32_t imped_kp_4;
    int32_t imped_ki_4;
    int32_t imped_kd_4;
    int32_t imped_kp_5;
    int32_t imped_ki_5;
    int32_t imped_kd_5;
    int32_t imped_kp_6;
    int32_t imped_ki_6;
    int32_t imped_kd_6;
    //导纳控制pid参数
    int32_t adnit_kp_1;
    int32_t adnit_ki_1;
    int32_t adnit_kd_1;
    int32_t adnit_kp_2;
    int32_t adnit_ki_2;
    int32_t adnit_kd_2;
    int32_t adnit_kp_3;
    int32_t adnit_ki_3;
    int32_t adnit_kd_3;
    int32_t adnit_kp_4;
    int32_t adnit_ki_4;
    int32_t adnit_kd_4;
    int32_t adnit_kp_5;
    int32_t adnit_ki_5;
    int32_t adnit_kd_5;
    int32_t adnit_kp_6;
    int32_t adnit_ki_6;
    int32_t adnit_kd_6;
}PARA_FORCE_STD_INFO_t;//(4+90)*4


/*参数需求表（伺服核《==》力控核）*/
typedef struct
{
/*每次参数更新时置1*/
    int32_t update_flag;
    int32_t rsv_data;
    int32_t posture[4];
    double  para_data[90];
    double  para_rsv[35];
}PARA_FORCE_INFO_t;		//1024 Bytes


/********************service and feedback**********************/
typedef struct
{
    int32_t cmd;		//reference SERVO_CMD_e
    int32_t param1;
    int32_t param2;
    int32_t param3;
    int32_t param4;
    int32_t param5;
    int32_t param6;
    int32_t param7;
}ServoCoreProcessCall_t;
typedef struct
{
    int32_t param1;
    int32_t param2;
    int32_t param3;
    int32_t param4;
    int32_t param5;
    int32_t param6;
    int32_t param7;
    int32_t param8;
}ForceCoreProcessBack_t;


/******************Instruction*****************/
typedef struct{
    int32_t valid;
    int32_t not_used;
    double in_pos_given[6];
    double in_vel_given[6];
    double in_acc_given[6];
    double in_force_given[6];

    double in_pos_back[6];
    double in_vel_back[6];
    double in_acc_back[6];
    double in_force_back[6];
}FORCE_INSTRUCTION_INFO_t;

/**************************result feedback*************************/
typedef struct{
    int32_t valid;
    int32_t err_code;		//reference FORCE_ERR_CODE_e
    int32_t err_history[4];	//reference FORCE_ERR_CODE_e
    int32_t state;			//reference FORCE_CORE_STATE_e
    int32_t mode;			//reference FORCE_WORK_MODE_e
    int32_t sub_mode;		//reference FORCE_WORK_SUB_MODE_e
    int32_t rsv_data;
}FORCE_CORE_STATE_t;

typedef struct{
    int32_t valid;
    int32_t theta[6];
    int32_t force[6];
    int32_t dignose_var[6][6];
    int32_t rsv_data;
}l,FORCE_BACK_INFO_t;

/*****************************enums********************************/
typedef enum FORCE_WORK_MODE
{
    NO_MODE = 0,
    IMPEDANCE_MODE = 1,
    ADMITTANCE_MODE = 2,
}FORCE_WORK_MODE_e;

typedef enum FORCE_WORK_SUB_MODE
{
    NO_SUB_MODE = 0,
    TEACH = 1,
    LEARN = 2,
}FORCE_WORK_SUB_MODE_e;

typedef enum{
    /*state machine control*/
    CMD_WORKING = 0x01,
    CMD_IDLE = 0x02,
    CMD_SET_FAULT = 0x03,
    CMD_RESET_FAULT = 0x04,
    CMD_SWITCH_MODE = 0x05,
    CMD_DIGNOSE_CONFIG = 0x06,
    CMD_GET_STATE = 0x07,

    CMD_SET_PARA = 0x0a,
    CMD_SET_INS = 0x0b,
    CMD_GET_RES = 0x0c,

    CMD_SHAKE_REQ = 0x0d,
    CMD_SHAKE_BIND = 0x0e,
    CMD_HEART_INC = 0x0f,

    CMD_VIRTUAL_FORCE = 0x10,
    CMD_SERVO_MAX = CMD_VIRTUAL_FORCE + 1,

}SERVO_CMD_e;

typedef enum{
    Cmd_resetFault=11,
    Cmd_idle=12,
    Cmd_working=13,
    Cmd_setFault=14,
    FORCE_GIVE_START=15,
    FORCE_GIVE_STOP=16,
}FORCE_CMD_e;

typedef enum{
    RES_INFO = 0xa0,
    RES_GET_STATE = 0xa1,
    RES_GET_RES = 0xa2,
    RES_DIAGNOSE_FILE= 0xa3,
    RES_SERVO_MAX = RES_DIAGNOSE_FILE + 1,
}SERVO_CMD_RES_e;

typedef enum
{
    NO_MODULE = 0x00,
    PARAM_MOUDLE = 0x01,
    ADNITTANCE_MOUDLE = 0x02,//dao na
    IMPEDANCE_MOUDLE = 0x03,//zu kang
    INSTRUCTION_MOUDLE = 0x04,//zhiling
    KINEMATIC_MOUDLE = 0x05,//forward kinematic
    KINEMATIC_INV_MOUDLE = 0x06,//inverse kinematic
    SERVO_COMM_MOUDLE = 0x07,//communication module
    DIFF_MOTION_MOUDLE = 0x08,
    MODULE_MAX = DIFF_MOTION_MOUDLE +1,
}FORCE_MODULE_e;

typedef enum
{
    HEART_ERR = 0x01,/*communication error*/
    CMD_ERR = 0x02,/*communication error*/
    INSTRUCTION_LOSE_ERR = 0x03,/*instruction buff empty*/
    INSTRUCTION_OVER_LIMIT_ERR = 0x04,/*instruction invalid*/
    RESULT_PUSH_ERR = 0x05,/*result buff full*/
    DIVED_ZERO = 0x06,
    KINEMATIC_CALC_ERR = 0x07,
    DIFF_CALC_ERR = 0x08,
    KINEMATIC_INV_CALC_ERR = 0x09,
}FORCE_ERR_CODE_e;

typedef enum
{
    POS_POSEULAR_GIVEN,
    THETA_BACK,
    THETA_BACK_TO_POS_EULAR,
    THETA_BACK_TO_POS_QUANT,
    OUT_POS_COMP,
    IN_FORCE_GIVEN_BASE,
    IN_FORCE_BACK_TOOL,
    IN_FORCE_BACK_BASE,
    FORCE_ERR_BASE,
    OUT_THETA,
    PID_ERR,
    PID_OUT,
    FVAR_MAX = PID_OUT + 1,
}FORCE_DIGNOSE_VAR_e;

typedef enum FORCE_CORE_STATE
{
    //SM_BOOT = 0,
    SM_INIT = 1,
    SM_IDLE = 2,
    SM_WORKING = 3,
    //SM_ABORT = 4,
    SM_ERROR = 5,
}FORCE_CORE_STATE_e;

typedef buffer_info_t SERVO_COMM_RINGS_BUFF_STRUCT;

/********************************************************************************/
/*comm with pc*/
#define MAX_DOFS 6
#define MAGIG_HEAD 0x11181118
typedef struct{
    int head;       //0x11181118
    int force_state;
    double force_theta[MAX_DOFS];
    //int check_sum;
}Pc_Comm_Data_t;

typedef struct{
    int cmd;
    int rsv;
    int params[MAX_DOFS];
}Pc_Comm_Cmd_t;

typedef struct{
    int valid;
    int force_state;
    double force_theta[MAX_DOFS];
}Force_Monitor_t;

typedef struct{
    int valid;
    int force[MAX_DOFS];
}Force_Virtual_Cmd_t;

/*
 *@brief the function is used to initiate force core communication
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_Init(void);
/**
 *@brief the function is used to get update flag of parameters
 *@param  void
 *@return void
 */
extern int Servo_Force_Comm_Get_Para_UpdateFlag(void);
/**
 *@brief the function is used to send shake-hand request to force core
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_ShakeHand_Request(void);
/**
 *@brief the function is used to bind communication with force-core
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_ShakeHand_Bind(void);
/**
 *@brief the function is used to update servo-core heart beat
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_ServoHeart_Inc(void);
/**
 *@brief the function is used to get force-core heart beat
 *@param  void
 *@return void
 */
extern int Servo_Force_Comm_Get_ForceHeart(void);
/**
 *@brief the function is used to enable force-core
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_Cmd_Enable(void);//change state from init to idle
/**
 *@brief the function is used to translate force-core state machine to working state
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_Cmd_Working(void);//change state from idle to working
/**
 *@brief the function is used to translate force-core state machine to idle state
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_Cmd_Idle(void);//change state from working to idle
/**
 *@brief the function is used to translate force-core state machine to abort state
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_Cmd_Abort(void);//change state from working to abort
/**
 *@brief the function is used to make force-core continue working
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_Cmd_Continue(void);
/**
 *@brief the function is used to disable force-core
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_Cmd_Disable(void);//change state from init to idle
/**
 *@brief the function is used to reset fault when force-core in fault state
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_Cmd_ResetFault(void);//change state from init to idle
extern void Servo_Force_Comm_Cmd_SetFault(void);
/**
 *@brief the function is used to set mode and submode of force-core
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_Cmd_Mode(int mode, int submode);
/**
 *@brief the function is used to configure diagnose data of force-core
 *@param  [in]mode
 *@param  [in]submode
 *@return void
 */
extern void Servo_Force_Comm_Cmd_DignoseCfg(FORCE_DIGNOSE_VAR_e *ptr);
/**
 *@brief the function is used to set parameters of force-core
 *@param [in]parameter struct pointer
 *@return void
 */
extern void Servo_Force_Comm_Set_Para(PARA_FORCE_STD_INFO_t *para_ptr);
/**
 *@brief the function is used to set instruction to force-core
 *@param  void
 *@return void
 */
extern int Servo_Force_Comm_Set_Instruct(FORCE_INSTRUCTION_INFO_t *push_ptr);

/**
 *@brief the function is used to get result of force-core state
 *@param  void
 *@return void
 */
extern int Servo_Force_Comm_Get_State(void);
/**
 *@brief the function is used to get result of force-core
 *@param  void
 *@return void
 */
extern int Servo_Force_Comm_Get_Result(FORCE_BACK_INFO_t *pull_ptr);
/**
 *@brief the function is used to discard history results of force-core
 *@param  void
 *@return void
 */
extern void Servo_Force_Comm_DiscardResult(void);

extern void *Servo_Adnit_Teach_Virtual_Force(void*);
extern void *Servo_Adnit_Teach_Virtual_Force_Trigger(void *p);

extern void Force_Monitor_Upload(Pc_Comm_Data_t *monitor);
extern void Force_Virtual_Download(Pc_Comm_Cmd_t *vcmd);
#endif

