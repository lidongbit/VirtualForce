#include "widget.h"
#include <QTimer>
extern QTimer timer;

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle(QString("Virtual Force"));
    w.show();
    return a.exec();
}
