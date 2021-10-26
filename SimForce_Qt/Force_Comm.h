#ifndef FORCE_COMM_H
#define FORCE_COMM_H
#include <QString>
#include <QMap>
#define MAX_DOFS 6
#define MAGIG_HEAD 0x11181118
typedef struct{
    int head;       //0x11181118
    int force_state;
    double force_theta[MAX_DOFS];
    //int check_sum;
}Force_Comm_Rcv_t;

const QString state_str[] = {
    "FORCE_BOOT",
    "FORCE_INIT",
    "FORCE_IDLE",
    "FORCE_WORKING",
    "FORCE_ABORT",
    "FORCE_FAULT"
};

typedef struct{
    int cmd;
    int rsv;
    int params[MAX_DOFS];
}Force_Comm_Send_t;

const QMap<QString,int> command = {
    std::map<QString,int>::value_type("Cmd_resetFault",11),
    std::map<QString,int>::value_type("Cmd_idle",12),
    std::map<QString,int>::value_type("Cmd_working",13),
    std::map<QString,int>::value_type("Cmd_setFault",14),
    std::map<QString,int>::value_type("FORCE_GIVE_START",15),
    std::map<QString,int>::value_type("FORCE_GIVE_STOP",16)
};
#endif // FORCE_COMM_H
