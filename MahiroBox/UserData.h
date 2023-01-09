#pragma once

#include <Windows.h>
#include <filesystem>
#include <string>
#include <set>

class UserData {
public:
	static UserData* instance();
	static void destroy();

	std::set<DWORD> left_key;
	std::set<DWORD> right_key;
	bool top_window;
	bool open_dialog;

	bool load(const std::string& filename);
	void save(const std::string& filename) const;
private:
	static UserData* m_instance;

	UserData();
};