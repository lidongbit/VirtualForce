#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include "Force_Comm.h"
//#include <QHostAddress>
class QHostAddress;
class QTcpServer;
class QTcpSocket;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void updateMonitor(void*);
    void setForceVal(void*);
    void sendMsg(void*);

private:
    quint16 port;
    QHostAddress *ipaddr;
    //QTcpServer *tcpServer;
    QTcpSocket *mClient = nullptr;      //客户端
    //QList<QTcpSocket *>mClientList;     //客户端链表
    Force_Comm_Rcv_t rcvMsg;
    Ui::Widget *ui;
private slots:
    void on_btn_connect_clicked();
    void connectedSlot();
    void disconnectedSlot();
    void readyReadSlot();
    void on_btn_resetFault_clicked();
    void on_btn_Idle_clicked();
    void on_btn_working_clicked();
    void on_btn_setFault_clicked();
    void on_btn_force_pressed();
    void on_btn_force_released();
    void updateShow(void);
    void on_btn_disconnect_clicked();
};
#endif // WIDGET_H
