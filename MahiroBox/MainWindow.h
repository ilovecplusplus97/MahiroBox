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
	virtual void closeEvent(QCloseEvent* e) override;
	virtual void keyPressEvent(QKeyEvent* e) override;

	void switch_wallpaper(bool wallpaper);
public:
    static MainWindow* current;
private:
    static HHOOK hKeyboardHook;
	static HHOOK hMouseHook;

    Ui::MainWindowClass ui;
	Qt::WindowFlags normal_flags;
	QSize normal_size;
	SettingWindow* setting_window;
	int left_pressed_count;
	int right_pressed_count;
	int mahiro_count;

    void load_resources();
	void load_userdata();

    static void init_hook();
    static void quit_hook();
    static LRESULT WINAPI KeyboardProc(INT code, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI MouseProc(INT code, WPARAM wParam, LPARAM lParam);
};
