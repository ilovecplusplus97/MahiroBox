#include "stdafx.h"
#include "Utility.h"
#include "tinyxml2.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::m_instance = nullptr;

ResourceManager* ResourceManager::instance() {
	if (m_instance == nullptr) {
		m_instance = new ResourceManager();
	}
	return m_instance;
}

void ResourceManager::destroy() {
	if (m_instance != nullptr) {
		delete m_instance;
		m_instance = nullptr;
	}
}

Resource* ResourceManager::load(const std::wstring& path) {
	return load(path, path);
}

Resource* ResourceManager::load(const std::wstring& path, const std::wstring& key) {
	if (!std::filesystem::exists(path)) {
		return nullptr;
	}
	auto it = m_resource.find(key);
	if (it != m_resource.end()) {
		return it->second;
	}
	std::wstring info_file = join(path, { L"info.xml" });
	if (!std::filesystem::exists(info_file)) {
		return nullptr;
	}
	tinyxml2::XMLDocument doc;
	const char* filename = wide_to_multi(info_file.c_str(), CP_ACP);
	doc.LoadFile(filename);
	delete filename;
	auto* root = doc.RootElement();
	auto* left = root->FirstChildElement("left");
	if (left == nullptr) {
		return nullptr;
	}
	auto* right = root->FirstChildElement("right");
	if (right == nullptr) {
		return nullptr;
	}
	auto* mahiro = root->FirstChildElement("mahiro");
	if (mahiro == nullptr) {
		return nullptr;
	}
	Resource* resource = new Resource();
	auto* image = left->FirstChildElement("image");
	int count = 0;
	do {
		if (image == nullptr) {
			delete resource;
			return nullptr;
		}
		const wchar_t* image_path_c = multi_to_wide(image->GetText(), CP_UTF8);
		std::wstring image_path = std::wstring(image_path_c);
		std::wstring image_path_full = join(path, { image_path });
		if (!std::filesystem::exists(image_path_full) && image_path != L"null") {
			delete resource;
			return nullptr;
		}
		if (image_path == L"null") {
			resource->right_hand[count] = nullptr;
		}
		resource->left_hand[count] = new QPixmap(QString::fromStdWString(image_path_full));
		image = image->NextSiblingElement("image");
		count++;
		delete image_path_c;
	} while (count < 2);
	image = right->FirstChildElement("image");
	count = 0;
	do {
		if (image == nullptr) {
			delete resource;
			return nullptr;
		}
		const wchar_t* image_path_c = multi_to_wide(image->GetText(), CP_UTF8);
		std::wstring image_path = std::wstring(image_path_c);
		std::wstring image_path_full = join(path, { image_path });
		if (!std::filesystem::exists(image_path_full) && image_path != L"null") {
			delete resource;
			return nullptr;
		}
		if (image_path == L"null") {
			resource->right_hand[count] = nullptr;
		}
		else {
			resource->right_hand[count] = new QPixmap(QString::fromStdWString(image_path_full));
		}
		image = image->NextSiblingElement("image");
		count++;
		delete image_path_c;
	} while (count < 2);
	for (image = mahiro->FirstChildElement("image"); image != nullptr; image = image->NextSiblingElement("image")) {
		const wchar_t* image_path_c = multi_to_wide(image->GetText(), CP_UTF8);
		std::wstring image_path = image_path_c;
		std::wstring image_full_path = join(path, { image_path });
		if (!std::filesystem::exists(image_full_path) && image_path != L"null") {
			delete resource;
			return nullptr;
		}
		if (image_path == L"null") {
		}
		else {
			resource->mahiro.push_back(new QPixmap(QString::fromStdWString(image_full_path)));
		}
		delete image_path_c;
	}
	return load(resource, key);
}

Resource* ResourceManager::load(Resource* resource, const std::wstring& key) {
	if (has(key)) {
		return resource;
	}
	m_resource.insert({ key, resource });
	return resource;
}

void ResourceManager::remove(const std::wstring& key) {
	auto it = m_resource.find(key);
	if (it != m_resource.end()) {
		delete it->second;
		m_resource.erase(it);
	}
}

void ResourceManager::remove(const Resource* resource) {
	auto remove_iterator = m_resource.end();
	for (auto it = m_resource.begin(); it != m_resource.end(); it++) {
		if (it->second == resource) {
			remove_iterator = it;
			break;
		}
	}
	if (remove_iterator != m_resource.end()) {
		delete remove_iterator->second;
		m_resource.erase(remove_iterator);
	}
}

Resource* ResourceManager::get(const std::wstring& key) const {
	if (!has(key)) {
		return nullptr;
	}
	return m_resource.at(key);
}

bool ResourceManager::has(const std::wstring& key) const {
	return m_resource.find(key) != m_resource.end();
}

bool ResourceManager::has(const Resource* resource) const {
	for (auto i : m_resource) { 
		if (i.second == resource) {
			return true;
		}
	}
	return false;
}

void ResourceManager::clear() {
	for (auto res : m_resource) {
		if (res.second != nullptr) {
			delete res.second;
		}
	}
	m_resource.clear();
}

ResourceManager::~ResourceManager() {
	clear();
}

Resource::Resource() :
	left_hand({ nullptr, nullptr }),
	right_hand({ nullptr, nullptr }),
	mahiro()
{
}

Resource::~Resource() {
	if (left_hand[0] != nullptr) {
		delete left_hand[0];
	}
	if (left_hand[1] != nullptr) {
		delete left_hand[1];
	}
	if (right_hand[0] != nullptr) {
		delete right_hand[0];
	}
	if (right_hand[1] != nullptr) {
		delete right_hand[1];
	}
	for (auto* p : mahiro) {
		if (p != nullptr) {
			delete p;
		}
	}
}
