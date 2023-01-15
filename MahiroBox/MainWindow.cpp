#include "stdafx.h"
#include "UserData.h"
#include "Utility.h"
#include "ResourceManager.h"
#include "SettingWindow.h"
#include "MainWindow.h"

MainWindow* MainWindow::current = nullptr;
HHOOK MainWindow::hKeyboardHook = nullptr;
HHOOK MainWindow::hMouseHook = nullptr;

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
	normal_size = size();
	normal_flags = windowFlags();

	load_userdata();
}

MainWindow::~MainWindow()
{
	quit_hook();
}

void MainWindow::paintEvent(QPaintEvent* e) {
	QPainter painter(this);
	Resource* resource = ResourceManager::instance()->get(UserData::instance()->style);
	QPointF scale(1.0f, 1.0f);
	for (int i = 0; i < resource->mahiro.size(); i++) {
		if (resource->mahiro[i] == nullptr) {
			continue;
		}
		scale.setX((qreal)width() / resource->mahiro[i]->width());
		scale.setY((qreal)height() / resource->mahiro[i]->height());
		break;
	}
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
	painter.drawPixmap(0, 0, width(), height(), *resource->mahiro[mahiro_index]);
	QPixmap* left_pixmap = left_pressed_count > 0 ? resource->left_hand[1] : resource->left_hand[0];
	if (left_pixmap != nullptr) {
		painter.drawPixmap(left_pixmap->width() * scale.x(), 0, width() / 2, height(), *left_pixmap);
	}
	QPixmap* right_pixmap = right_pressed_count > 0 ? resource->right_hand[1] : resource->right_hand[0];
	if (right_pixmap != nullptr) {
		painter.drawPixmap(0, 0, width() / 2, height(), *right_pixmap);
	}
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* e) {
	switch (e->button()) {
	case Qt::LeftButton: {
		if (setting_window != nullptr) {
			setting_window->close();
			delete setting_window;
		}
		setting_window = new SettingWindow(nullptr);
		setting_window->show();
		break;
	}
	case Qt::RightButton: {
		close();
		break;
	}
	default:
		break;
	}
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
	
}

void MainWindow::switch_wallpaper(bool wallpaper) {
	if (wallpaper) {
		RECT desktop_rect;
		SystemParametersInfoW(SPI_GETWORKAREA, 0, &desktop_rect, 0);
		MoveWindow((HWND)winId(), 0, 0, desktop_rect.right, desktop_rect.bottom, TRUE);
		SetParent((HWND)winId(), get_wallpaper_window());
	}
	else {
		resize(normal_size);
		move(100, 100);
		SetParent((HWND)winId(), nullptr);
	}
}

void MainWindow::load_userdata() {
	UserData* userdata = UserData::instance();
	Qt::WindowFlags flags(normal_flags);
	if (userdata->top_window) {
		flags = flags | Qt::WindowStaysOnTopHint;
	}
	if (userdata->is_wallpaper) {
		flags = flags | Qt::FramelessWindowHint;
	}
	qDebug() << flags << " " << normal_flags;
	this->hide();
	setWindowFlags(flags);
	this->show();
	switch_wallpaper(userdata->is_wallpaper);
}

void MainWindow::init_hook() {
	if (hKeyboardHook == nullptr) {
		hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
	}
	if (hMouseHook == nullptr) {
		hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, nullptr, 0);
	}
}

void MainWindow::quit_hook() {
	if (hKeyboardHook != nullptr) {
		UnhookWindowsHookEx(hKeyboardHook);
		hKeyboardHook = nullptr;
	}
	if (hMouseHook != nullptr) {
		UnhookWindowsHookEx(hMouseHook);
		hMouseHook = nullptr;
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

LRESULT WINAPI MainWindow::MouseProc(INT code, WPARAM wParam, LPARAM lParam) {
	static INT64 begintime = -1;
	INT64 nowtime = GetTickCount();
	std::wstring window_text(256, L'\0');
	GetClassNameW(GetForegroundWindow(), window_text.data(), 256);
	if (
		!UserData::instance()->is_wallpaper || 
		window_text[0] != L'W' || window_text[1] != L'o' || window_text[2] != L'r' || window_text[3] != L'k' || 
		window_text[4] != L'e' || window_text[5] != L'r' || window_text[6] != L'W') 
	{
		return CallNextHookEx(hMouseHook, code, wParam, lParam);
	}
	auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
	if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN) {
		if (begintime == -1) {
			begintime = GetTickCount();
		}
		else {
			if (nowtime - begintime <= 200) {
				QMouseEvent* event = new QMouseEvent(
					QEvent::MouseButtonDblClick, 
					QPointF(info->pt.x, info->pt.y), 
					QPointF(info->pt.x, info->pt.y), 
					wParam == WM_LBUTTONDOWN ? Qt::LeftButton : Qt::RightButton, 
					Qt::MouseButtons(), 
					Qt::KeyboardModifier::NoModifier);
				MainWindow::current->mouseDoubleClickEvent(event);
				delete event;
			}
			begintime = -1;
		}
	}
	return CallNextHookEx(hMouseHook, code, wParam, lParam);
}