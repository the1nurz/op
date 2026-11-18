#include <cassert>
#include <cwchar>
#include "MainWindow.h"
#include "resource.h"

namespace {
BOOL CALLBACK FindEditorWindow(HWND hwnd, LPARAM data) {
    wchar_t name[64]{};
    GetClassNameW(hwnd, name, 64);
    if (std::wcscmp(name, L"Lab5ShapeEditor") == 0) {
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
    SendMessageW(hwnd, WM_COMMAND, IDM_TABLE, 0);
    HWND table = GetWindow(hwnd, GW_ENABLEDPOPUP);
    assert(table && table != hwnd && IsWindowEnabled(hwnd));
    HWND list = GetDlgItem(table, IDC_SHAPES);
    assert(list && ListView_GetItemCount(list) == 0);
    // Малювання при відкритому немодальному діалозі.
    for (int i = 0; i < 6; ++i) {
        SendMessageW(hwnd, WM_COMMAND, 10+i, 0);
        SendMessageW(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(100+i*20, 150));
        SendMessageW(hwnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(120+i*20, 180));
        SendMessageW(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(120+i*20, 180));
    }
    assert(MyEditor::Instance().Count() == 6 && ListView_GetItemCount(list) == 6);
    ListView_SetItemState(list, 4, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    assert(MyEditor::Instance().Selected() == 4);
    assert(IsWindowEnabled(GetDlgItem(table, IDC_DELETE_SHAPE)));
    const Shape* retained = MyEditor::Instance().At(5);
    SendMessageW(table, WM_COMMAND, IDC_DELETE_SHAPE, 0);
    assert(MyEditor::Instance().Count() == 5 && ListView_GetItemCount(list) == 5);
    assert(MyEditor::Instance().At(4) == retained && MyEditor::Instance().Selected() == -1);
    // Перемальовування виділення не повинне накопичувати GDI-об'єкти.
    UpdateWindow(hwnd);
    DWORD before = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    for (int i = 0; i < 200; ++i) {
        MyEditor::Instance().Select(i%5);
        InvalidateRect(hwnd, nullptr, TRUE);
        UpdateWindow(hwnd);
    }
    assert(GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS) <= before);
    SendMessageW(table, WM_CLOSE, 0, 0);
    assert(!IsWindow(table) && IsWindow(hwnd));
    assert(!(GetMenuState(GetMenu(hwnd), IDM_TABLE, MF_BYCOMMAND) & MF_CHECKED));
    SendMessageW(hwnd, WM_COMMAND, IDM_TABLE, 0);
    table = GetWindow(hwnd, GW_ENABLEDPOPUP);
    assert(table != hwnd && ListView_GetItemCount(GetDlgItem(table, IDC_SHAPES)) == 5);
    // Повторне натискання закриває єдину таблицю, наступне відкриває її.
    SendMessageW(hwnd, WM_COMMAND, IDM_TABLE, 0);
    assert(!IsWindow(table));
    SendMessageW(hwnd, WM_COMMAND, IDM_TABLE, 0);
    table = GetWindow(hwnd, GW_ENABLEDPOPUP);
    assert(table != hwnd);
    DestroyWindow(hwnd);
    assert(!IsWindow(table) && MyEditor::Instance().Count() == 0);
    return 0;
}
