#pragma once

#include <QtWidgets/QMainWindow>
#include <Windows.h>
#include <iostream>
#include <format>
#include "ui_MainWindow.h"

class SettingWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	friend class SettingWindow;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual void paintEvent(QPaintEvent* e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* e) override;
public:
    static MainWindow* current;
private:
    static HHOOK hKeyboardHook;

    Ui::MainWindowClass ui;
    std::vector<QPixmap*> m_mahiro_images;
    std::vector<QPixmap*> m_left_hand_images;
    std::vector<QPixmap*> m_right_hand_images;
	SettingWindow* setting_window;
	int left_pressed_count;
	int right_pressed_count;
	int mahiro_count;

    void load_resources();
	void load_userdata();

    static void init_hook();
    static void quit_hook();
    static LRESULT WINAPI KeyboardProc(INT code, WPARAM wParam, LPARAM lParam);
};
