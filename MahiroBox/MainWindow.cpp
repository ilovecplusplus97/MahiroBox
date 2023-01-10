#include "stdafx.h"
#include "UserData.h"
#include "ResourceManager.h"
#include "SettingWindow.h"
#include "MainWindow.h"

MainWindow* MainWindow::current = nullptr;
HHOOK MainWindow::hKeyboardHook = nullptr;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent),
	left_pressed_count(0),
	right_pressed_count(0),
	mahiro_count(0),
	setting_window(nullptr)
{
	ui.setupUi(this);
	init_hook();
	current = this;

	setFixedSize(size());
	load_userdata();
}

MainWindow::~MainWindow()
{
	quit_hook();
}

void MainWindow::paintEvent(QPaintEvent* e) {
	QPainter painter(this);
	Resource* resource = ResourceManager::instance()->get(UserData::instance()->style);
	if (resource == nullptr) {
		return;
	}
	int mahiro_image_count = resource->mahiro.size();
	int mahiro_index = 0;
	if ((mahiro_count / mahiro_image_count) % 2 == 0) {
		mahiro_index = mahiro_count % mahiro_image_count;
	}
	else {
		mahiro_index = mahiro_image_count - 1 - (mahiro_count % mahiro_image_count);
	}
	painter.drawPixmap(0, 0, *resource->mahiro[mahiro_index]);
	QPixmap* left_pixmap = left_pressed_count > 0 ? resource->left_hand[1] : resource->left_hand[0];
	if (left_pixmap != nullptr) {
		painter.drawPixmap(left_pixmap->width(), 0, *left_pixmap);
	}
	QPixmap* right_pixmap =  right_pressed_count > 0 ? resource->right_hand[1] : resource->right_hand[0];
	if (right_pixmap != nullptr) {
		painter.drawPixmap(0, 0, *right_pixmap);
	}
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* e) {
	if (setting_window != nullptr) {
		setting_window->close();
		delete setting_window;
	}
	setting_window = new SettingWindow(this);
	setting_window->show();
}

void MainWindow::load_userdata() {
	UserData* userdata = UserData::instance();
	Qt::WindowFlags flags = Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowFullscreenButtonHint;
	if (userdata->top_window) {
		flags = flags | Qt::WindowStaysOnTopHint;
	}
	this->hide();
	setWindowFlags(flags);
	this->show();
}

void MainWindow::init_hook() {
	if (hKeyboardHook == nullptr) {
		hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
	}
}

void MainWindow::quit_hook() {
	if (hKeyboardHook != nullptr) {
		UnhookWindowsHookEx(hKeyboardHook);
		hKeyboardHook = nullptr;
	}
}

LRESULT WINAPI MainWindow::KeyboardProc(INT code, WPARAM wParam, LPARAM lParam) {
	static std::set<DWORD> left_pressed_keys;
	static std::set<DWORD> right_pressed_keys;

	if (UserData::instance()->open_dialog) {
		return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
	}
	auto* info = (KBDLLHOOKSTRUCT*)lParam;
	if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
		if (MainWindow::current->setting_window != nullptr) {
			if (MainWindow::current->setting_window->selected_left()) {
				if (UserData::instance()->left_key.count(info->vkCode) == 0) {
					UserData::instance()->left_key.insert(info->vkCode);
					MainWindow::current->setting_window->add_left_key(info->vkCode, info->scanCode);
					MainWindow::current->setting_window->unselect_left();
				}
			}
			else if (MainWindow::current->setting_window->selected_right()) {
				if (UserData::instance()->right_key.count(info->vkCode) == 0) {
					UserData::instance()->right_key.insert(info->vkCode);
					MainWindow::current->setting_window->add_right_key(info->vkCode, info->scanCode);
					MainWindow::current->setting_window->unselect_right();
				}
			}
		}

		if (UserData::instance()->left_key.count(info->vkCode) > 0) {
			if (left_pressed_keys.count(info->vkCode) == 0) {
				MainWindow::current->left_pressed_count++;
				left_pressed_keys.insert(info->vkCode);
			}
			MainWindow::current->mahiro_count++;
			MainWindow::current->update();
		}
		if (UserData::instance()->right_key.count(info->vkCode) > 0) {
			if (right_pressed_keys.count(info->vkCode) == 0) {
				MainWindow::current->right_pressed_count++;
				right_pressed_keys.insert(info->vkCode);
			}
			MainWindow::current->mahiro_count++;
			MainWindow::current->update();
		}
	}
	else {
		if (UserData::instance()->left_key.count(info->vkCode) > 0) {
			left_pressed_keys.erase(info->vkCode);
			MainWindow::current->left_pressed_count--;
			MainWindow::current->update();
		}
		if (UserData::instance()->right_key.count(info->vkCode) > 0) {
			right_pressed_keys.erase(info->vkCode);
			MainWindow::current->right_pressed_count--;
			MainWindow::current->update();
		}
	}
	return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
}
