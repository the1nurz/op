#include "module2.h"
#include "module2_resources.h"

namespace {
ResultCallback resultCallback = nullptr;

INT_PTR CALLBACK TextDialogProcedure(HWND dialog, UINT message, WPARAM wParam, LPARAM)
{
    switch (message) {
    case WM_INITDIALOG:
        SendDlgItemMessageW(dialog, IDC_TEXT_INPUT, EM_SETLIMITTEXT, 4096, 0);
        SetFocus(GetDlgItem(dialog, IDC_TEXT_INPUT));
        return FALSE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            HWND input = GetDlgItem(dialog, IDC_TEXT_INPUT);
            int length = GetWindowTextLengthW(input);
            std::wstring value(static_cast<size_t>(length) + 1, L'\0');
            int copied = GetWindowTextW(input, &value[0], length + 1);
            value.resize(static_cast<size_t>(copied));
            HWND owner = GetWindow(dialog, GW_OWNER);
            ResultCallback callback = resultCallback;
            DestroyWindow(dialog);
            if (callback) {
                callback(owner, L"Введений текст: " + value);
            }
            return TRUE;
        }
        case IDCANCEL:
            DestroyWindow(dialog);
            return TRUE;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(dialog);
        return TRUE;
    }
    return FALSE;
}
}

HWND ShowTextDialog(HWND owner, ResultCallback onResult)
{
    resultCallback = onResult;
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(owner, GWLP_HINSTANCE));
    HWND dialog = CreateDialogParamW(instance, MAKEINTRESOURCEW(IDD_TEXT_DIALOG),
        owner, TextDialogProcedure, 0);
    if (dialog) {
        ShowWindow(dialog, SW_SHOW);
    }
    return dialog;
}
