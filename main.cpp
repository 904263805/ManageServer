#include "ManageServer.h"
#include <QtWidgets/QApplication>
#include "ManageSql.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ManageServer w;
    w.show();
    ManageSql sql;
    return a.exec();
}
