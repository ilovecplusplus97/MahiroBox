#include "stdafx.h"
#include "UserData.h"
#include "MainWindow.h"
#include "ResourceManager.h"
#include <QtWidgets/QApplication>

const std::string USERDATA_FILENAME = "userdata.xml";

int main(int argc, char *argv[]) {
	HANDLE hMutex = CreateMutexW(nullptr, FALSE, L"MahiroBoxMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBoxW(nullptr, L"已经有其他Mahiro Box实例在运行，请先关闭其他Mahiro Box实例再运行", L"错误", MB_OK | MB_ICONERROR);
		return 0;
	}

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
