#include "stdafx.h"
#include "UserData.h"
#include "Utility.h"
#include "MainWindow.h"
#include "ResourceManager.h"
#include "SettingWindow.h"

SettingWindow::SettingWindow(QWidget *parent)
	: QDialog(parent),
	m_selected_left(false),
	m_selected_right(false)
{
	ui.setupUi(this);

	m_old_left_text = ui.W2LeftKeyAddButton->text();
	m_old_right_text = ui.W2RightKeyAddButton->text();
	load_userdata();
	set_focus(this);
}

SettingWindow::~SettingWindow()
{
	
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

void SettingWindow::load_userdata() {
	ui.W1LeftKeyList->clear();
	ui.W1RightKeyList->clear();
	for (auto i : UserData::instance()->left_key) {
		add_left_key(i, MapVirtualKey(i, MAPVK_VK_TO_VSC));
	}
	for (auto i : UserData::instance()->right_key) {
		add_right_key(i, MapVirtualKey(i, MAPVK_VK_TO_VSC));
	}
	ui.W1TopWindowCheck->setChecked(UserData::instance()->top_window && !UserData::instance()->is_wallpaper);
	std::filesystem::directory_iterator dir("mahiro_style");
	for (auto& i : dir) {
		if (!i.is_directory()) {
			continue;
		}
		ui.W2StyleCombo->addItem(QString::fromStdWString(standard_path(i.path().wstring())));
	}
	ui.W2WallpaperCheck->setChecked(UserData::instance()->is_wallpaper);
	ui.W1TopWindowCheck->setDisabled(UserData::instance()->is_wallpaper);
}

void SettingWindow::set_focus(QWidget* widget) {
	for (auto* i : widget->children()) {
		if (i->isWidgetType()) {
			QWidget* child = qobject_cast<QWidget*>(i);
			child->setFocusPolicy(Qt::NoFocus);
			set_focus(child);
		}
	}
}

void SettingWindow::closeEvent(QCloseEvent*) {
	m_selected_left = false;
	m_selected_right = false;
}

void SettingWindow::keyPressEvent(QKeyEvent* e) {
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return || e->key() == Qt::Key_Tab || Qt::Key_Escape) {
		return;
	}
	QDialog::keyPressEvent(e);
}

void SettingWindow::on_W1LoadButton_clicked() {
	UserData::instance()->open_dialog = true;
	QString filename = QFileDialog::getOpenFileName(this, "选择文件", QString(), "XML配置文件(*.xml)");
	UserData::instance()->open_dialog = false;
	if (filename.isEmpty()) {
		return;
	}
	if (!UserData::instance()->load(filename.toStdString())) {
		UserData::instance()->open_dialog = true;
		QMessageBox::critical(this, "错误", "文件非法", QMessageBox::Ok);
		UserData::instance()->open_dialog = false;
		return;
	}
	load_userdata();
	MainWindow::current->load_userdata();
}

void SettingWindow::on_W2SaveButton_clicked() {
	UserData::instance()->open_dialog = true;
	QString filename = QFileDialog::getSaveFileName(this, "保存文件", QString(), "XML配置文件(*.xml)");
	UserData::instance()->open_dialog = false;
	if (filename.isEmpty()) {
		return;
	}
	UserData::instance()->save(filename.toStdString());
}

void SettingWindow::on_W1TopWindowCheck_clicked() {	
	UserData::instance()->top_window = ui.W1TopWindowCheck->isChecked();
	MainWindow::current->load_userdata();
}

void SettingWindow::on_W2WallpaperCheck_clicked() {
	auto* userdata = UserData::instance();
	userdata->is_wallpaper = ui.W2WallpaperCheck->isChecked();
	if (userdata->is_wallpaper) {
		userdata->top_window = false;
		ui.W1TopWindowCheck->setChecked(false);
		ui.W1TopWindowCheck->setDisabled(true);
		if (!userdata->not_show_wallpaper_info) {
			userdata->not_show_wallpaper_info = QMessageBox::information(this, "提示", "在桌面双击右键退出软件", "确定", "不再提示") == 1;
		}
	}
	else {
		ui.W1TopWindowCheck->setDisabled(false);
	}
	MainWindow::current->load_userdata();
}

void SettingWindow::on_W2StyleCombo_currentTextChanged(QString path) {
	if (path.toStdWString() == UserData::instance()->style) {
		return;
	}
	
	UserData::instance()->style = path.toStdWString();
	ResourceManager::instance()->load(path.toStdWString());
	MainWindow::current->update();
}
