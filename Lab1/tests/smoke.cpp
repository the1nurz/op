// Тестуємо реальну віконну процедуру, результати та ресурси головного модуля.
#include "../main.cpp"
#include "../module1_resources.h"
#include "../module2_resources.h"
#include <iostream>
#include <stdexcept>

namespace {
void Check(bool condition, const char* description)
{
    if (!condition) {
        throw std::runtime_error(description);
    }
    std::cout << "PASS: " << description << '\n';
}

void SelectWork(HWND owner, int command)
{
    SendMessageW(owner, WM_COMMAND, static_cast<WPARAM>(command), 0);
}
}

int main()
{
    HWND owner = nullptr;
    try {
        HINSTANCE instance = GetModuleHandleW(nullptr);
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = WindowProcedure;
        windowClass.hInstance = instance;
        windowClass.lpszMenuName = MAKEINTRESOURCEW(IDR_MAIN_MENU);
        windowClass.lpszClassName = WindowClassName;
        Check(RegisterClassExW(&windowClass) != 0, "register main window");
        owner = CreateWindowExW(0, WindowClassName, L"Lab1 test", WS_OVERLAPPEDWINDOW,
            0, 0, 720, 400, nullptr, nullptr, instance, nullptr);
        Check(owner != nullptr, "create main window with menu resource");
        Check(GetMenuItemCount(GetMenu(owner)) == 2, "exactly two menu items");

        SelectWork(owner, IDM_WORK1);
        HWND groupDialog = activeDialog;
        Check(IsWindow(groupDialog) != FALSE, "create group dialog from resource");
        HWND list = GetDlgItem(groupDialog, IDC_GROUP_LIST);
        Check(SendMessageW(list, LB_GETCOUNT, 0, 0) == 7, "automatically populate group list");
        Check(!IsWindowEnabled(GetDlgItem(groupDialog, IDOK)), "disable confirmation before selection");
        std::wstring previous = result;
        SendMessageW(groupDialog, WM_COMMAND, IDOK, 0);
        Check(IsWindow(groupDialog) && result == previous, "ignore confirmation without selection");
        SendMessageW(list, LB_SETCURSEL, 1, 0);
        SendMessageW(groupDialog, WM_COMMAND, MAKEWPARAM(IDC_GROUP_LIST, LBN_SELCHANGE),
            reinterpret_cast<LPARAM>(list));
        Check(IsWindowEnabled(GetDlgItem(groupDialog, IDOK)) != FALSE, "enable confirmation after selection");
        SendMessageW(groupDialog, WM_COMMAND, IDOK, 0);
        Check(!IsWindow(groupDialog) && result == L"Вибрана група: ІО-42", "confirm and display selected group");

        SelectWork(owner, IDM_WORK2);
        HWND textDialog = activeDialog;
        Check(IsWindow(textDialog) != FALSE, "create text dialog from resource");
        SetDlgItemTextW(textDialog, IDC_TEXT_INPUT, L"Привіт, ФІОТ! & 27");
        SendMessageW(textDialog, WM_COMMAND, IDOK, 0);
        Check(!IsWindow(textDialog) && result == L"Введений текст: Привіт, ФІОТ! & 27",
            "confirm Unicode text with ampersand");

        previous = result;
        SelectWork(owner, IDM_WORK2);
        SetDlgItemTextW(activeDialog, IDC_TEXT_INPUT, L"Скасований текст");
        SendMessageW(activeDialog, WM_COMMAND, IDCANCEL, 0);
        Check(!IsWindow(activeDialog) && result == previous, "cancel preserves previous result");

        SelectWork(owner, IDM_WORK1);
        groupDialog = activeDialog;
        SelectWork(owner, IDM_WORK2);
        Check(GetDlgItem(activeDialog, IDC_TEXT_INPUT) != nullptr,
            "switch to text dialog while group dialog is open");
        // Windows може повторно використати HWND; перевіряємо тип вікна за контролом.
        Check(!IsWindow(groupDialog) || GetDlgItem(groupDialog, IDC_GROUP_LIST) == nullptr,
            "previous group dialog is removed");
        textDialog = activeDialog;
        SendMessageW(textDialog, WM_CLOSE, 0, 0);
        Check(!IsWindow(textDialog) && result == previous, "close button preserves previous result");

        SelectWork(owner, IDM_WORK2);
        SendMessageW(activeDialog, WM_COMMAND, IDOK, 0);
        Check(result == L"Введений текст: ", "accept empty text safely");
        for (int i = 0; i < 20; ++i) {
            SelectWork(owner, i % 2 == 0 ? IDM_WORK1 : IDM_WORK2);
            Check(IsWindow(activeDialog) != FALSE, "reopen dialog");
        }
        textDialog = activeDialog;
        SendMessageW(owner, WM_CLOSE, 0, 0);
        Check(!IsWindow(owner) && !IsWindow(textDialog), "close application and active dialog");
        std::cout << "All smoke checks passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        CloseActiveDialog();
        if (IsWindow(owner)) {
            DestroyWindow(owner);
        }
        return 1;
    }
}
