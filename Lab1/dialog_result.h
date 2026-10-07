#pragma once
#include <windows.h>
#include <string>

// Спільний контракт результату, без залежності між модулями діалогів.
using ResultCallback = void (*)(HWND owner, const std::wstring& value);
