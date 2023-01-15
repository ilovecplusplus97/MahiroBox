#include "stdafx.h"
#include "Utility.h"

static HWND workerW;

std::wstring get_scancode_name(DWORD scancode) {
    WCHAR keyname[16];
    if (GetKeyNameTextW((LONG)scancode << 16L, keyname, 16) == 0) {
        return std::format(L"scancode: {}", scancode);
    }
    return keyname;
}

std::string standard_path(const std::string& path) {
	std::string res = path;
	std::replace(res.begin(), res.end(), '\\', '/');
	return res;
}

std::wstring standard_path(const std::wstring& path) {
	std::wstring res = path;
	std::replace(res.begin(), res.end(), L'\\', L'/');
	return res;
}

std::string join(const std::string& a, const std::vector<std::string>& b) {
	std::filesystem::path path(a);
	for (auto& i : b) {
		path.append(i);
	}
	return standard_path(path.string());
}

std::wstring join(const std::wstring& a, const std::vector<std::wstring>& b) {
	std::filesystem::path path(a);
	for (auto& i : b) {
		path.append(i);
	}
	return standard_path(path.wstring());
}

char* wide_to_multi(const wchar_t* str, DWORD code_page) {
	size_t length = WideCharToMultiByte(code_page, 0, str, -1, nullptr, 0, nullptr, nullptr);
	char* res = new char[length];
	WideCharToMultiByte(code_page, 0, str, -1, res, length, nullptr, nullptr);
	return res;
}

wchar_t* multi_to_wide(const char* str, DWORD code_page) {
	size_t length = MultiByteToWideChar(code_page, 0, str, -1, nullptr, 0);
	wchar_t* res = new wchar_t[length];
	MultiByteToWideChar(code_page, 0, str, -1, res, length);
	return res;
}

HWND get_wallpaper_window() {
	HWND wallpaper_window = nullptr;
	if (wallpaper_window == nullptr) {
		int result;
		wallpaper_window = FindWindowW(L"Progman", nullptr);
		SendMessageTimeout(wallpaper_window, 0x052c, 0, 0, SMTO_NORMAL, 0x3e8, (PDWORD_PTR)&result);
		EnumWindows(EnumWindowProc, (LPARAM)nullptr);
		ShowWindow(workerW, SW_HIDE);
	}
	return wallpaper_window;
}

HWND get_workerw_window() {
	get_wallpaper_window();
	return workerW;
}

static BOOL CALLBACK EnumWindowProc(HWND hwnd, LPARAM lParam) {
	HWND defview = FindWindowExW(hwnd, 0, L"SHELLDLL_DefView", nullptr);
	if (defview != nullptr) {
		workerW = FindWindowExW(0, hwnd, L"WorkerW", 0);;
	}
	return true;
}