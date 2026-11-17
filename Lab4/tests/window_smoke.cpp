#include <cassert>
#include <cwchar>
#include "MainWindow.h"

namespace {
BOOL CALLBACK FindEditorWindow(HWND hwnd, LPARAM data) {
    wchar_t name[64]{};
    GetClassNameW(hwnd, name, 64);
    if (std::wcscmp(name, L"Lab4ShapeEditor") == 0) {
        *reinterpret_cast<HWND*>(data) = hwnd;
        return FALSE;
    }
    return TRUE;
}
}

int main() {
    MainWindow window;
    assert(window.Create(GetModuleHandleW(nullptr), SW_HIDE));
    HWND hwnd = nullptr;
    EnumThreadWindows(GetCurrentThreadId(), FindEditorWindow, reinterpret_cast<LPARAM>(&hwnd));
    assert(hwnd);
    HWND toolbar = FindWindowExW(hwnd, nullptr, TOOLBARCLASSNAMEW, nullptr);
    assert(toolbar);
    assert(SendMessageW(toolbar, TB_BUTTONCOUNT, 0, 0) == 6);
    HIMAGELIST images = reinterpret_cast<HIMAGELIST>(SendMessageW(toolbar, TB_GETIMAGELIST, 0, 0));
    assert(images && ImageList_GetImageCount(images) == 6);
    HMENU menu = GetSubMenu(GetMenu(hwnd), 1);
    assert(menu && GetMenuItemCount(menu) == 6);
    assert(GetMenuState(menu, 10, MF_BYCOMMAND) & MF_CHECKED);
    assert(SendMessageW(toolbar, TB_ISBUTTONCHECKED, 10, 0));

    const wchar_t* names[] = {L"Крапка", L"Лінія", L"Прямокутник", L"Еліпс",
        L"Лінія з кружечками", L"Каркас куба"};
    HWND tooltip = reinterpret_cast<HWND>(SendMessageW(toolbar, TB_GETTOOLTIPS, 0, 0));
    assert(tooltip);
    for (int tool = 10; tool <= 15; ++tool) {
        SendMessageW(hwnd, WM_COMMAND, tool, 0);
        for (int other = 10; other <= 15; ++other) {
            assert(!!(GetMenuState(menu, other, MF_BYCOMMAND) & MF_CHECKED) == (other == tool));
            assert(!!SendMessageW(toolbar, TB_ISBUTTONCHECKED, other, 0) == (other == tool));
        }
        NMTTDISPINFOW info{};
        info.hdr.hwndFrom = tooltip;
        info.hdr.idFrom = tool;
        info.hdr.code = TTN_GETDISPINFOW;
        SendMessageW(hwnd, WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&info));
        assert(info.lpszText && std::wcscmp(info.lpszText, names[tool - 10]) == 0);
    }
    DestroyWindow(hwnd);
    return 0;
}
