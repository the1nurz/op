#include "MyTable.h"
#include "../resources/resource.h"
#include <commctrl.h>

void MyTable::SetCallback(Callback callback, void* context) noexcept {
    callback_ = callback;
    context_ = context;
}

bool MyTable::Open(HINSTANCE instance, HWND owner) {
    if (hwnd_) { ShowWindow(hwnd_, SW_SHOW); SetForegroundWindow(hwnd_); return true; }
    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_LISTVIEW_CLASSES};
    if (!InitCommonControlsEx(&controls)) return false;
    HWND dialog = CreateDialogParamW(instance, MAKEINTRESOURCEW(IDD_TABLE), owner,
        DialogProc, reinterpret_cast<LPARAM>(this));
    if (!dialog) return false;
    ShowWindow(dialog, SW_SHOW);
    return true;
}

void MyTable::Close() { if (hwnd_) DestroyWindow(hwnd_); }
bool MyTable::ProcessMessage(MSG& message) const {
    return hwnd_ && IsDialogMessageW(hwnd_, &message);
}
void MyTable::Notify(Event event, int row) {
    if (callback_) callback_(context_, event, row);
}

void MyTable::SetRows(const std::vector<TableRow>& rows, int selected) {
    if (!list_) return;
    updating_ = true;
    SendMessageW(list_, WM_SETREDRAW, FALSE, 0);
    ListView_DeleteAllItems(list_);
    for (size_t i = 0; i < rows.size(); ++i) {
        std::wstring number = std::to_wstring(i+1);
        LVITEMW item{};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(i);
        item.pszText = number.data();
        const int row = ListView_InsertItem(list_, &item);
        const auto& r = rows[i];
        std::wstring cells[] = {r.type, std::to_wstring(r.x1), std::to_wstring(r.y1),
            std::to_wstring(r.x2), std::to_wstring(r.y2)};
        for (int col = 0; col < 5; ++col)
            ListView_SetItemText(list_, row, col+1, cells[col].data());
    }
    if (selected >= 0 && selected < static_cast<int>(rows.size())) {
        ListView_SetItemState(list_, selected, LVIS_SELECTED | LVIS_FOCUSED,
            LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(list_, selected, FALSE);
    }
    EnableWindow(GetDlgItem(hwnd_, IDC_DELETE_SHAPE), selected >= 0);
    SendMessageW(list_, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(list_, nullptr, TRUE);
    updating_ = false;
}

void MyTable::Resize() {
    if (!list_) return;
    RECT r{};
    GetClientRect(hwnd_, &r);
    const int width = (r.right > 280 ? r.right : 280);
    const int height = (r.bottom > 100 ? r.bottom : 100);
    MoveWindow(list_, 10, 10, width-20, height-52, TRUE);
    MoveWindow(GetDlgItem(hwnd_, IDC_DELETE_SHAPE), 10, height-32, 160, 24, TRUE);
    MoveWindow(GetDlgItem(hwnd_, IDCANCEL), width-105, height-32, 95, 24, TRUE);
}

INT_PTR CALLBACK MyTable::DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* self = reinterpret_cast<MyTable*>(GetWindowLongPtrW(hwnd, DWLP_USER));
    if (message == WM_INITDIALOG) {
        self = reinterpret_cast<MyTable*>(lParam);
        self->hwnd_ = hwnd;
        SetWindowLongPtrW(hwnd, DWLP_USER, reinterpret_cast<LONG_PTR>(self));
    }
    return self ? self->Dispatch(message, wParam, lParam) : FALSE;
}

INT_PTR MyTable::Dispatch(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG: {
        list_ = GetDlgItem(hwnd_, IDC_SHAPES);
        SendMessageW(list_, CCM_SETUNICODEFORMAT, TRUE, 0);
        ListView_SetExtendedListViewStyle(list_, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
        const wchar_t* names[] = {L"№", L"Тип фігури", L"x1", L"y1", L"x2", L"y2"};
        const int widths[] = {40, 190, 70, 70, 70, 70};
        for (int i = 0; i < 6; ++i) {
            LVCOLUMNW column{};
            column.mask = LVCF_TEXT | LVCF_WIDTH;
            column.pszText = const_cast<wchar_t*>(names[i]);
            column.cx = widths[i];
            ListView_InsertColumn(list_, i, &column);
        }
        EnableWindow(GetDlgItem(hwnd_, IDC_DELETE_SHAPE), FALSE);
        Resize();
        return TRUE;
    }
    case WM_GETMINMAXINFO: {
        auto* info = reinterpret_cast<MINMAXINFO*>(lParam);
        info->ptMinTrackSize = {400, 180};
        return TRUE;
    }
    case WM_SIZE: Resize(); return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL) { Close(); return TRUE; }
        if (LOWORD(wParam) == IDC_DELETE_SHAPE) {
            int row = ListView_GetNextItem(list_, -1, LVNI_SELECTED);
            if (row >= 0) Notify(Event::Delete, row);
            return TRUE;
        }
        break;
    case WM_NOTIFY: {
        const auto* header = reinterpret_cast<NMHDR*>(lParam);
        if (!header || header->hwndFrom != list_ || updating_) break;
        if (header->code == LVN_ITEMCHANGED) {
            const auto* change = reinterpret_cast<NMLISTVIEW*>(lParam);
            if ((change->uChanged & LVIF_STATE)
                && ((change->uOldState ^ change->uNewState) & LVIS_SELECTED)) {
                const int row = ListView_GetNextItem(list_, -1, LVNI_SELECTED);
                EnableWindow(GetDlgItem(hwnd_, IDC_DELETE_SHAPE), row >= 0);
                Notify(Event::Select, row);
            }
        } else if (header->code == LVN_KEYDOWN) {
            const auto* key = reinterpret_cast<NMLVKEYDOWN*>(lParam);
            const int row = ListView_GetNextItem(list_, -1, LVNI_SELECTED);
            if (key->wVKey == VK_DELETE && row >= 0) Notify(Event::Delete, row);
        }
        return TRUE;
    }
    case WM_CLOSE: Close(); return TRUE;
    case WM_NCDESTROY:
        SetWindowLongPtrW(hwnd_, DWLP_USER, 0);
        hwnd_ = nullptr;
        list_ = nullptr;
        Notify(Event::Closed, -1);
        return FALSE;
    }
    return FALSE;
}
