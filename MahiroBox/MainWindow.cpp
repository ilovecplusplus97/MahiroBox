#include "stdafx.h"
#include "UserData.h"
#include "SettingWindow.h"
#include "MainWindow.h"

constexpr int mahiro_image_count = 7;

MainWindow* MainWindow::current = nullptr;
HHOOK MainWindow::hKeyboardHook = nullptr;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent),
	left_pressed(false),
	right_pressed(false),
	mahiro_count(0),
	setting_window(nullptr)
{
	ui.setupUi(this);
	init_hook();
	current = this;

	setFixedSize(size());
	load_resources();
}

MainWindow::~MainWindow()
{
	for (auto* image : m_mahiro_images) {
		if (image != nullptr) {
			delete image;
		}
	}
	for (auto* image : m_left_hand_images) {
		if (image != nullptr) {
			delete image;
		}
	}
	for (auto* image : m_right_hand_images) {
		if (image != nullptr) {
			delete image;
		}
	}
	quit_hook();
}

void MainWindow::paintEvent(QPaintEvent* e) {
	QPainter painter(this);
	painter.drawPixmap(0, 0, *(m_mahiro_images[mahiro_count % mahiro_image_count]));
	QPixmap* left_pixmap = left_pressed ? m_left_hand_images[1] : m_left_hand_images[0];
	if (left_pixmap) {
		painter.drawPixmap(left_pixmap->width(), 0, *left_pixmap);
	}
	QPixmap* right_pixmap = right_pressed ? m_right_hand_images[1] : m_right_hand_images[0];
	if (right_pixmap) {
		painter.drawPixmap(0, 0, *right_pixmap);
	}
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* e) {
	if (setting_window != nullptr) {
		delete setting_window;
	}
	setting_window = new SettingWindow(this);
	setting_window->exec();
}

void MainWindow::load_resources() {
	static QString mahiro_path_fmt = ":/Resources/%1.png";
	for (int i = 1; i <= mahiro_image_count; i++) {
		m_mahiro_images.push_back(new QPixmap(mahiro_path_fmt.arg(i)));
	}
	m_left_hand_images.push_back(new QPixmap(":/Resources/left.png"));
	m_left_hand_images.push_back(nullptr);
	m_right_hand_images.push_back(new QPixmap(":/Resources/right.png"));
	m_right_hand_images.push_back(new QPixmap(":/Resources/right_pressed.png"));
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
			MainWindow::current->left_pressed = true;
			MainWindow::current->mahiro_count++;
			MainWindow::current->update();
		}
		if (UserData::instance()->right_key.count(info->vkCode) > 0) {
			MainWindow::current->right_pressed = true;
			MainWindow::current->mahiro_count++;
			MainWindow::current->update();
		}
	}
	else {
		if (UserData::instance()->left_key.count(info->vkCode) > 0) {
			MainWindow::current->left_pressed = false;
			MainWindow::current->update();
		}
		if (UserData::instance()->right_key.count(info->vkCode) > 0) {
			MainWindow::current->right_pressed = false;
			MainWindow::current->update();
		}
	}
	return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
}
