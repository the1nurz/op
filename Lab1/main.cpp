#include <windows.h>
#include <string>
#include "module1.h"
#include "module2.h"
#include "resource.h"

namespace {
constexpr wchar_t WindowClassName[] = L"Lab1MainWindow";
HWND activeDialog = nullptr;
std::wstring result = L"Оберіть «Робота1» або «Робота2» у меню.";

void ShowResult(HWND owner, const std::wstring& value)
{
    result = value;
    InvalidateRect(owner, nullptr, TRUE);
}

void CloseActiveDialog()
{
    if (IsWindow(activeDialog)) {
        DestroyWindow(activeDialog);
    }
    activeDialog = nullptr;
}

LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_WORK1:
        case IDM_WORK2:
            // Немодальні діалоги дозволяють обрати іншу роботу через меню.
            // Попередній діалог завжди закривається перед створенням нового.
            CloseActiveDialog();
            activeDialog = LOWORD(wParam) == IDM_WORK1
                ? ShowGroupDialog(window, ShowResult)
                : ShowTextDialog(window, ShowResult);
            if (!activeDialog) {
                MessageBoxW(window, L"Не вдалося створити діалогове вікно.",
                    L"Lab1 — помилка", MB_OK | MB_ICONERROR);
            }
            return 0;
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(window, &paint);
        RECT area{};
        GetClientRect(window, &area);
        area.left += 24;
        area.top += 24;
        area.right -= 24;
        area.bottom -= 24;
        HGDIOBJ previousFont = SelectObject(dc, GetStockObject(DEFAULT_GUI_FONT));
        SetBkMode(dc, TRANSPARENT);
        DrawTextW(dc, L"Лабораторна робота №1. Варіант 27\n"
            L"Робота1 — вибір групи; Робота2 — введення тексту.",
            -1, &area, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
        area.top += 70;
        DrawTextW(dc, result.c_str(), -1, &area,
            DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
        SelectObject(dc, previousFont);
        EndPaint(window, &paint);
        return 0;
    }
    case WM_SIZE:
        InvalidateRect(window, nullptr, TRUE);
        return 0;
    case WM_CLOSE:
        CloseActiveDialog();
        DestroyWindow(window);
        return 0;
    case WM_DESTROY:
        CloseActiveDialog();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand)
{
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = instance;
    windowClass.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    windowClass.hIconSm = windowClass.hIcon;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = MAKEINTRESOURCEW(IDR_MAIN_MENU);
    windowClass.lpszClassName = WindowClassName;
    if (!RegisterClassExW(&windowClass)) {
        MessageBoxW(nullptr, L"Не вдалося зареєструвати головне вікно.",
            L"Lab1 — помилка", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND window = CreateWindowExW(0, WindowClassName, L"Lab1 — Варіант 27",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 720, 400,
        nullptr, nullptr, instance, nullptr);
    if (!window) {
        MessageBoxW(nullptr, L"Не вдалося створити головне вікно.",
            L"Lab1 — помилка", MB_OK | MB_ICONERROR);
        return 1;
    }
    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG message{};
    BOOL status;
    while ((status = GetMessageW(&message, nullptr, 0, 0)) > 0) {
        // Tab, Enter та Esc обробляються як у звичайному Windows-діалозі.
        if (!IsWindow(activeDialog) || !IsDialogMessageW(activeDialog, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    return status == -1 ? 1 : static_cast<int>(message.wParam);
}
