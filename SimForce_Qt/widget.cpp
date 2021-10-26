#include "widget.h"
#include "ui_widget.h"
#include <QTcpServer>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMessageBox>
#include <QByteArray>
#include <QCheckBox>
#include <QTimer>
#include <string.h>
#include <QNetworkInterface>
#include <QMessageBox>
QTimer timer;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
#if 0
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach(QHostAddress addr,list)
    {
        if(addr.protocol() == QAbstractSocket::IPv4Protocol)
        {
            if(addr.toString().contains("192.168.110."))
            {
                ipaddr = new QHostAddress(addr.toString());
                break;
            }
        }
    }
    if(ipaddr == nullptr)
    {
        QMessageBox::warning(this,"Error","Please add ip like 192.168.110.*!");
    }
#endif
    mClient = new QTcpSocket();
    ipaddr = new QHostAddress(" 192.168.110.2");
    port = 8811;
    memset(&this->rcvMsg, 0 ,sizeof(Force_Comm_Rcv_t));
    //tcpServer = new QTcpServer();
    timer.start(1000*0.1);
    // 新建连接
    //connect(tcpServer, &QTcpServer::newConnection, this, &Widget::newConnectionSlot);
    connect(&timer,SIGNAL(timeout()), this, SLOT(updateShow()));
    connect(mClient, SIGNAL(readyRead()),this, SLOT(readyReadSlot()));          //接收消息
    connect(mClient, SIGNAL(connected()),this, SLOT(connectedSlot()));    //断开连接
    connect(mClient, SIGNAL(disconnected()),this, SLOT(disconnectedSlot()));    //断开连接
}

Widget::~Widget()
{
    delete ui;
}

void Widget::disconnectedSlot()
{
    //QMessageBox::information(this, "client close Signal", "client over");
    ui->lab_info->clear();
    ui->btn_disconnect->setEnabled(false);
    ui->btn_connect->setEnabled(true);
}
void Widget::connectedSlot()
{
    //QMessageBox::information(this, "client close Signal", "client over");
    ui->lab_info->setText("Connect Success!");
    ui->btn_connect->setEnabled(false);
    ui->btn_disconnect->setEnabled(true);
}
void Widget::readyReadSlot()
{
    QByteArray recvArray;
    Force_Comm_Rcv_t *pdata = nullptr;
    int len = 0;


    recvArray = mClient->readAll();
    if(recvArray.isEmpty()) return;
    pdata = (Force_Comm_Rcv_t*)recvArray.data();
    len = recvArray.size();

    if(pdata->head == MAGIG_HEAD && len == sizeof(Force_Comm_Rcv_t))
    {
        //updateMonitor(pdata);
        memcpy(&rcvMsg, pdata, sizeof(Force_Comm_Rcv_t));
    }

#if 0
        //转发给其他客户端
        for(int index = 0;index < mClientList.count();index ++)
        {
            QTcpSocket* temp = mClientList.at(index);
            if(current == temp) continue;
            if(temp->isWritable())
            {
                temp->write(recvArray);
            }
        }
#endif

}
void Widget::updateMonitor(void *data_ptr)
{
    static int t = 0;

    Force_Comm_Rcv_t *pdata =(Force_Comm_Rcv_t*)data_ptr;
    //qDebug()<<"2 time count: "<<t++<<" "<<pdata->force_theta[0];
    ui->lab_force_state->setText(state_str[pdata->force_state]);
    ui->theta_axis_0->setText(QString::number(pdata->force_theta[0],'f'));
    ui->theta_axis_1->setText(QString::number(pdata->force_theta[1],'f'));
    ui->theta_axis_2->setText(QString::number(pdata->force_theta[2],'f'));
    ui->theta_axis_3->setText(QString::number(pdata->force_theta[3],'f'));
    ui->theta_axis_4->setText(QString::number(pdata->force_theta[4],'f'));
    ui->theta_axis_5->setText(QString::number(pdata->force_theta[5],'f'));
    //ui->layout_monitor->update();
}


void Widget::sendMsg(void *pdata)
{
    QByteArray sendArr = QByteArray((char*)pdata,sizeof(Force_Comm_Send_t));
    Force_Comm_Send_t *p = (Force_Comm_Send_t*)pdata;

    mClient->write(sendArr);
    qDebug()<<p->cmd<<p->params[0]<<p->params[1]<<p->params[2]<<p->params[3]<<p->params[4]<<p->params[5];
}

void Widget::on_btn_resetFault_clicked()
{
    Force_Comm_Send_t sendData;
    memset(&sendData, 0, sizeof(Force_Comm_Send_t));
    sendData.cmd = command["Cmd_resetFault"];
    sendMsg(&sendData);
}

void Widget::on_btn_Idle_clicked()
{
    Force_Comm_Send_t sendData;
    memset(&sendData, 0, sizeof(Force_Comm_Send_t));
    sendData.cmd = command["Cmd_idle"];
    sendMsg(&sendData);
}

void Widget::on_btn_working_clicked()
{
    Force_Comm_Send_t sendData;
    memset(&sendData, 0, sizeof(Force_Comm_Send_t));
    sendData.cmd = command["Cmd_working"];
    sendMsg(&sendData);
}

void Widget::on_btn_setFault_clicked()
{
    Force_Comm_Send_t sendData;
    memset(&sendData, 0, sizeof(Force_Comm_Send_t));
    sendData.cmd = command["Cmd_setFault"];
    sendMsg(&sendData);
}


void Widget::on_btn_force_pressed()
{
    Force_Comm_Send_t sendData;
    memset(&sendData, 0, sizeof(Force_Comm_Send_t));
    sendData.cmd = command["FORCE_GIVE_START"];
    setForceVal(&sendData);
    sendMsg(&sendData);
}

void Widget::on_btn_force_released()
{
    Force_Comm_Send_t sendData;
    memset(&sendData, 0, sizeof(Force_Comm_Send_t));
    sendData.cmd = command["FORCE_GIVE_STOP"];
    sendMsg(&sendData);
}

void Widget::setForceVal(void *psend)
{
    Force_Comm_Send_t *pdata = (Force_Comm_Send_t*)psend;
    if(ui->cbx_axis_0->isChecked()==true)
    {
        pdata->params[0] = ui->lineEdit_1->text().toDouble()*16777216;
    }
    if(ui->cbx_axis_1->isChecked()==true)
    {
        pdata->params[1] = ui->lineEdit_2->text().toDouble()*16777216;
    }
    if(ui->cbx_axis_2->isChecked()==true)
    {
        pdata->params[2] = ui->lineEdit_3->text().toDouble()*16777216;
    }
    if(ui->cbx_axis_3->isChecked()==true)
    {
        pdata->params[3] = ui->lineEdit_4->text().toDouble()*16777216;
    }
    if(ui->cbx_axis_4->isChecked()==true)
    {
        pdata->params[4] = ui->lineEdit_5->text().toDouble()*16777216;
    }
    if(ui->cbx_axis_5->isChecked()==true)
    {
        pdata->params[5] = ui->lineEdit_6->text().toDouble()*16777216;
    }
}

void Widget::updateShow(void)
{
    static int t = 0;
    //qDebug()<<"1 time count: "<<t++<<" "<<rcvMsg.force_theta[0];
    updateMonitor(&rcvMsg);
}

void Widget::on_btn_disconnect_clicked()
{
    if(mClient->state()==QAbstractSocket::ConnectedState)
    {
        mClient->disconnectFromHost();
    }
    ui->btn_disconnect->setEnabled(false);
    ui->btn_connect->setEnabled(true);
}
void Widget::on_btn_connect_clicked()
{
    mClient->connectToHost(*ipaddr,port);
}
