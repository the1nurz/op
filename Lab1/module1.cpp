#include "module1.h"
#include "module1_resources.h"

namespace {
ResultCallback resultCallback = nullptr;
constexpr const wchar_t* Groups[] = {
    L"ІО-41", L"ІО-42", L"ІО-43", L"ІП-41", L"ІП-42", L"ІС-41", L"ІС-42"
};

// Анонімний namespace приховує callback від інших одиниць трансляції.
INT_PTR CALLBACK GroupDialogProcedure(HWND dialog, UINT message, WPARAM wParam, LPARAM)
{
    switch (message) {
    case WM_INITDIALOG: {
        HWND list = GetDlgItem(dialog, IDC_GROUP_LIST);
        for (const wchar_t* group : Groups) {
            SendMessageW(list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(group));
        }
        EnableWindow(GetDlgItem(dialog, IDOK), FALSE);
        SetFocus(list);
        return FALSE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_GROUP_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                EnableWindow(GetDlgItem(dialog, IDOK),
                    SendDlgItemMessageW(dialog, IDC_GROUP_LIST, LB_GETCURSEL, 0, 0) != LB_ERR);
                return TRUE;
            }
            break;
        case IDOK: {
            HWND list = GetDlgItem(dialog, IDC_GROUP_LIST);
            LRESULT index = SendMessageW(list, LB_GETCURSEL, 0, 0);
            if (index == LB_ERR) {
                return TRUE;
            }
            LRESULT length = SendMessageW(list, LB_GETTEXTLEN, static_cast<WPARAM>(index), 0);
            if (length == LB_ERR) {
                return TRUE;
            }
            std::wstring value(static_cast<size_t>(length) + 1, L'\0');
            SendMessageW(list, LB_GETTEXT, static_cast<WPARAM>(index),
                reinterpret_cast<LPARAM>(&value[0]));
            value.resize(static_cast<size_t>(length));
            HWND owner = GetWindow(dialog, GW_OWNER);
            ResultCallback callback = resultCallback;
            DestroyWindow(dialog);
            if (callback) {
                callback(owner, L"Вибрана група: " + value);
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

HWND ShowGroupDialog(HWND owner, ResultCallback onResult)
{
    resultCallback = onResult;
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(owner, GWLP_HINSTANCE));
    HWND dialog = CreateDialogParamW(instance, MAKEINTRESOURCEW(IDD_GROUP_DIALOG),
        owner, GroupDialogProcedure, 0);
    if (dialog) {
        ShowWindow(dialog, SW_SHOW);
    }
    return dialog;
}
