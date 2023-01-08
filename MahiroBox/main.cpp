#include "stdafx.h"
#include "UserData.h"
#include "MainWindow.h"
#include <QtWidgets/QApplication>

const std::string USERDATA_FILENAME = "userdata.xml";

int main(int argc, char *argv[])
{
    UserData::instance()->load(USERDATA_FILENAME);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int code = a.exec();
    UserData::instance()->save(USERDATA_FILENAME);
    return code;
}
