#include "stdafx.h"
#include "UserData.h"
#include "MainWindow.h"
#include "ResourceManager.h"
#include <QtWidgets/QApplication>

const std::string USERDATA_FILENAME = "userdata.xml";

int main(int argc, char *argv[])
{
    UserData::instance()->load(USERDATA_FILENAME);
    QApplication a(argc, argv);
	if (ResourceManager::instance()->load(UserData::instance()->style) == nullptr) {
		QMessageBox::critical(nullptr, "错误", "风格非法", QMessageBox::Ok);
		ResourceManager::instance()->load(L"mahiro_style/default");
	}
    MainWindow w;
    w.show();
    int code = a.exec();
    UserData::instance()->save(USERDATA_FILENAME);
	ResourceManager::destroy();
	UserData::destroy();
    return code;
}
