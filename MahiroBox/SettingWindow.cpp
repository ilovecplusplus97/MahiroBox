#include "stdafx.h"
#include "UserData.h"
#include "Utility.h"
#include "MainWindow.h"
#include "SettingWindow.h"

SettingWindow::SettingWindow(QWidget *parent)
	: QDialog(parent),
	m_selected_left(false),
	m_selected_right(false)
{
	ui.setupUi(this);

	m_old_left_text = ui.W2LeftKeyAddButton->text();
	m_old_right_text = ui.W2RightKeyAddButton->text();
	load_keys();
}

SettingWindow::~SettingWindow()
{
	m_selected_left = false;
	m_selected_right = false;
}

bool SettingWindow::selected_left() const {
	return m_selected_left;
}

bool SettingWindow::selected_right() const {
	return m_selected_right;
}

void SettingWindow::select_left() {
	ui.W2LeftKeyAddButton->setText("按下任意键");
	ui.W2LeftKeyAddButton->setEnabled(false);
	m_selected_left = true;
}

void SettingWindow::select_right() {
	ui.W2RightKeyAddButton->setText("按下任意键");
	ui.W2RightKeyAddButton->setEnabled(false);
	m_selected_right = true;
}

void SettingWindow::unselect_left() {
	ui.W2LeftKeyAddButton->setText(m_old_left_text);
	ui.W2LeftKeyAddButton->setEnabled(true);
	m_selected_left = false;
}

void SettingWindow::unselect_right() {
	ui.W2RightKeyAddButton->setText(m_old_right_text);
	ui.W2RightKeyAddButton->setEnabled(true);
	m_selected_right = false;
}

void SettingWindow::add_left_key(DWORD vkcode, DWORD scancode) {
	KeyListWidgetItem* item = new KeyListWidgetItem(QString::fromStdWString(get_scancode_name(scancode)), ui.W1LeftKeyList);
	item->vkcode = vkcode;
	item->scancode = scancode;
}

void SettingWindow::add_right_key(DWORD vkcode, DWORD scancode) {
	KeyListWidgetItem* item = new KeyListWidgetItem(QString::fromStdWString(get_scancode_name(scancode)), ui.W1RightKeyList);
	item->vkcode = vkcode;
	item->scancode = scancode;
}


void SettingWindow::on_W2LeftKeyAddButton_clicked() {
	if (selected_right()) {
		unselect_right();
	}
	select_left();
}

void SettingWindow::on_W2RightKeyAddButton_clicked() {
	if (selected_left()) {
		unselect_left();
	}
	select_right();
}

void SettingWindow::on_W1LeftKeyList_itemDoubleClicked(QListWidgetItem* item) {
	ui.W1LeftKeyList->removeItemWidget(item);
	auto* key_item = dynamic_cast<KeyListWidgetItem*>(item);
	UserData::instance()->left_key.erase(key_item->vkcode);
	delete key_item;
}

void SettingWindow::on_W1RightKeyList_itemDoubleClicked(QListWidgetItem* item) {
	ui.W1RightKeyList->removeItemWidget(item);
	auto* key_item = dynamic_cast<KeyListWidgetItem*>(item);
	UserData::instance()->right_key.erase(key_item->vkcode);
	delete key_item;
}

void SettingWindow::load_keys() {
	ui.W1LeftKeyList->clear();
	ui.W1RightKeyList->clear();
	for (auto i : UserData::instance()->left_key) {
		add_left_key(i, MapVirtualKey(i, MAPVK_VK_TO_VSC));
	}
	for (auto i : UserData::instance()->right_key) {
		add_right_key(i, MapVirtualKey(i, MAPVK_VK_TO_VSC));
	}
}

void SettingWindow::on_W1LoadButton_clicked() {
	QString filename = QFileDialog::getOpenFileName(this, "选择文件");
	if (filename.isEmpty()) {
		return;
	}
	if (!UserData::instance()->load(filename.toStdString())) {
		QMessageBox::critical(this, "错误", "文件非法", QMessageBox::Ok);
		return;
	}
	load_keys();
}

void SettingWindow::on_W2SaveButton_clicked() {
	QString filename = QFileDialog::getSaveFileName(this, "保存文件");
	if (filename.isEmpty()) {
		return;
	}
	UserData::instance()->save(filename.toStdString());
}