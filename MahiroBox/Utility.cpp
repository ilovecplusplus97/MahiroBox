#include "stdafx.h"
#include "Utility.h"

std::wstring get_scancode_name(DWORD scancode) {
    WCHAR keyname[16];
    if (GetKeyNameTextW((LONG)scancode << 16L, keyname, 16) == 0) {
        return std::format(L"scancode: {}", scancode);
    }
    return keyname;
}