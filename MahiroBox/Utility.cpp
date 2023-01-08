#include "stdafx.h"
#include "Utility.h"

std::wstring get_scancode_name(DWORD scancode) {
    WCHAR keyname[16];
    if (GetKeyNameText((LONG)scancode << 16L + (1L << 25L), keyname, 16) == 0) {
        return std::format(L"scancode: {}", scancode);
    }
    return keyname;
}