#pragma once

#include <QMainWindow>
#include "ui_SettingWindow.h"

class KeyListWidgetItem : public QListWidgetItem {
public:
	using QListWidgetItem::QListWidgetItem;

	DWORD vkcode;
	DWORD scancode;
};

class SettingWindow : public QDialog
{
	Q_OBJECT

public:
	SettingWindow(QWidget *parent = nullptr);
	~SettingWindow();

	bool selected_left() const;
	bool selected_right() const;
	void select_left();
	void select_right();
	void unselect_left();
	void unselect_right();
	void add_left_key(DWORD vkcode, DWORD scancode);
	void add_right_key(DWORD vkcode, DWORD scancode);

	void closeEvent(QCloseEvent*) override;
	void reject() override;
private slots:
	void on_W2LeftKeyAddButton_clicked();
	void on_W2RightKeyAddButton_clicked();
	void on_W1LeftKeyList_itemDoubleClicked(QListWidgetItem* item);
	void on_W1RightKeyList_itemDoubleClicked(QListWidgetItem* item);
	void on_W1LoadButton_clicked();
	void on_W2SaveButton_clicked();
	void on_W1TopWindowCheck_clicked();
private:
	Ui::SettingWindowClass ui;

	QString m_old_left_text;
	QString m_old_right_text;
	bool m_selected_left;
	bool m_selected_right;

	void load_userdata();
};

