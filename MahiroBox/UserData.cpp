#include "stdafx.h"
#include "tinyxml2.h"
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
	left_key.clear();
	right_key.clear();
	for (auto* i = left->FirstChildElement(); i != nullptr; i = i->NextSiblingElement()) {
		left_key.insert(i->Unsigned64Text());
	}
	for (auto* i = right->FirstChildElement(); i != nullptr; i = i->NextSiblingElement()) {
		right_key.insert(i->Unsigned64Text());
	}
	top_window = topwindow->BoolText();
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

	doc.SaveFile(filename.c_str());
}

UserData::UserData() :
	left_key(),
	right_key(),
	top_window(false)
{

}