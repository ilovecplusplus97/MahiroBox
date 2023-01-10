#pragma once

#include <Windows.h>
#include <filesystem>
#include <vector>
#include <string>
#include <format>

std::wstring get_scancode_name(DWORD scancode);
std::string standard_path(const std::string& path);
std::wstring standard_path(const std::wstring& path);
std::string join(const std::string& a, const std::vector<std::string>& b);
std::wstring join(const std::wstring& a, const std::vector<std::wstring>& b);
char* wide_to_multi(const wchar_t* str, DWORD code_page);
wchar_t* multi_to_wide(const char* str, DWORD code_page);