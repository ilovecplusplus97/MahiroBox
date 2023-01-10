#include "stdafx.h"
#include "tinyxml2.h"
#include "Utility.h"
#include "UserData.h"

UserData* UserData::m_instance = nullptr;

UserData* UserData::instance() {
	if (m_instance == nullptr) {
		m_instance = new UserData();
	}
	return m_instance;
}

void UserData::destroy() {
	if (m_instance != nullptr) {
		delete m_instance;
		m_instance = nullptr;
	}
}

bool UserData::load(const std::string& filename) {
	if (!std::filesystem::exists(filename)) {
		return false;
	}
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str());
	auto* root = doc.RootElement();
	auto* left = root->FirstChildElement("left");
	if (left == nullptr) {
		return false;
	}
	auto* right = root->FirstChildElement("right");
	if (right == nullptr) {
		return false;
	}
	auto* topwindow = root->FirstChildElement("topwindow");
	if (topwindow == nullptr) {
		return false;
	}
	auto* style = root->FirstChildElement("style");
	if (style == nullptr) {
		return false;
	}
	left_key.clear();
	right_key.clear();
	for (auto* i = left->FirstChildElement(); i != nullptr; i = i->NextSiblingElement()) {
		left_key.insert(i->Unsigned64Text());
	}
	for (auto* i = right->FirstChildElement(); i != nullptr; i = i->NextSiblingElement()) {
		right_key.insert(i->Unsigned64Text());
	}
	top_window = topwindow->BoolText();
	const wchar_t* s = multi_to_wide(style->GetText(), CP_UTF8);
	this->style = s;
	delete s;
	return true;
}

void UserData::save(const std::string& filename) const {
	tinyxml2::XMLDocument doc;
	auto* root = doc.NewElement("root");
	doc.InsertEndChild(root);

	auto* left = doc.NewElement("left");
	root->InsertEndChild(left);
	for (auto i : left_key) {
		auto* key = doc.NewElement("key");
		key->SetText((unsigned long long)i);
		left->InsertEndChild(key);
	}

	auto* right = doc.NewElement("right");
	root->InsertEndChild(right);
	for (auto i : right_key) {
		auto* key = doc.NewElement("key");
		key->SetText((unsigned long long)i);
		right->InsertEndChild(key);
	}

	auto* topwindow = doc.NewElement("topwindow");
	root->InsertEndChild(topwindow);
	topwindow->SetText(top_window);

	auto* style = doc.NewElement("style");
	root->InsertEndChild(style);
	char* s = wide_to_multi(this->style.c_str(), CP_UTF8);
	style->SetText(s);

	doc.SaveFile(filename.c_str());
	delete s;
}

UserData::UserData() :
	left_key(),
	right_key(),
	open_dialog(false),
	top_window(false),
	style(L"mahiro_style/default")
{

}