#include "MainWindow.h"
#include "../resources/resource.h"
#include <windowsx.h>
#include <commdlg.h>
#include <vector>
#include "ShapeFactory.h"
namespace {
constexpr wchar_t kWindowClassName[] = L"Lab5ShapeEditor";
constexpr int kFirstToolId = 10;
constexpr wchar_t kWindowTitle[] = L"Lab5 - \u0433\u0440\u0430\u0444\u0456\u0447\u043d\u0438\u0439 \u0440\u0435\u0434\u0430\u043a\u0442\u043e\u0440";
}

bool MainWindow::Create(HINSTANCE instance, int showCommand) {
    instance_ = instance;
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = &MainWindow::WndProc;
    windowClass.hInstance = instance_;
    windowClass.lpszClassName = kWindowClassName;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    if (!RegisterClassW(&windowClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;

    HMENU menu = CreateMenuBar();
    if (!menu) return false;
    hwnd_ = CreateWindowW(kWindowClassName, kWindowTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 960, 700, nullptr, menu, instance_, this);
    if (!hwnd_) {
        DestroyMenu(menu);
        return false;
    }
    ShowWindow(hwnd_, showCommand);
    UpdateWindow(hwnd_);
    return true;
}

int MainWindow::Run() {
    MSG message{};
    int result;
    while ((result = GetMessageW(&message, nullptr, 0, 0)) > 0) {
        if (table_.ProcessMessage(message)) continue;
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return result == -1 ? 1 : static_cast<int>(message.wParam);
}

HMENU MainWindow::CreateMenuBar() const {
    HMENU menu = CreateMenu();
    HMENU file = CreatePopupMenu();
    HMENU objects = CreatePopupMenu();
    HMENU help = CreatePopupMenu();
    if (!menu || !file || !objects || !help) {
        if (menu) DestroyMenu(menu);
        if (file) DestroyMenu(file);
        if (objects) DestroyMenu(objects);
        if (help) DestroyMenu(help);
        return nullptr;
    }
    if (!AppendMenuW(file, MF_STRING, IDM_OPEN, L"Відкрити...")) goto fail;
    if (!AppendMenuW(file, MF_STRING, IDM_SAVE, L"Зберегти...")) goto fail;
    if (!AppendMenuW(file, MF_SEPARATOR, 0, nullptr)) goto fail;
    if (!AppendMenuW(file, MF_STRING, 1, L"\u0412\u0438\u0445\u0456\u0434")) goto fail;
    {
        int count = 0;
        const auto* types = ShapeTypes(count);
        for (int i = 0; i < count; ++i)
            if (!AppendMenuW(objects, MF_STRING, types[i].id, types[i].name)) goto fail;
    }
    if (!AppendMenuW(help, MF_STRING, 2, L"\u041f\u0440\u043e \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u0443")) goto fail;
    if (!AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(file), L"\u0424\u0430\u0439\u043b")) goto fail;
    file = nullptr;
    if (!AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(objects), L"\u041e\u0431\u0027\u0454\u043a\u0442\u0438")) goto fail;
    objects = nullptr;
    if (!AppendMenuW(menu, MF_STRING, IDM_TABLE, L"Таблиця об’єктів")) goto fail;
    if (!AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(help), L"\u0414\u043e\u0432\u0456\u0434\u043a\u0430")) goto fail;
    help = nullptr;
    return menu;

fail:
    if (menu) DestroyMenu(menu);
    if (file) DestroyMenu(file);
    if (objects) DestroyMenu(objects);
    if (help) DestroyMenu(help);
    return nullptr;
}

bool MainWindow::CreateToolbar() {
    int typeCount = 0;
    const auto* types = ShapeTypes(typeCount);
    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_BAR_CLASSES};
    if (!InitCommonControlsEx(&controls)) return false;
    toolbar_ = CreateWindowExW(0, TOOLBARCLASSNAMEW, nullptr,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_TOP | CCS_NOPARENTALIGN,
        0, 0, 0, 0, hwnd_, nullptr, instance_, nullptr);
    if (!toolbar_) return false;

    SendMessageW(toolbar_, CCM_SETUNICODEFORMAT, TRUE, 0);
    SendMessageW(toolbar_, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    imageList_ = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, typeCount, 0);
    if (!imageList_) return false;

    for (int i = 0; i < typeCount; ++i) {
        int id = types[i].bitmapId;
        HBITMAP bitmap = static_cast<HBITMAP>(LoadImageW(instance_, MAKEINTRESOURCEW(id), IMAGE_BITMAP,
            16, 16, LR_CREATEDIBSECTION));
        if (!bitmap) return false;
        const int imageIndex = ImageList_AddMasked(imageList_, bitmap, RGB(255, 255, 255));
        DeleteObject(bitmap);
        if (imageIndex < 0) return false;
    }
    SendMessageW(toolbar_, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(imageList_));
    SendMessageW(toolbar_, TB_SETBUTTONSIZE, 0, MAKELPARAM(24, 24));

    std::vector<TBBUTTON> buttons(typeCount);
    for (int i = 0; i < typeCount; ++i) {
        buttons[i].iBitmap = i;
        buttons[i].idCommand = types[i].id;
        buttons[i].fsState = TBSTATE_ENABLED;
        buttons[i].fsStyle = BTNS_CHECKGROUP | BTNS_AUTOSIZE;
        buttons[i].iString = -1;
    }
    if (!SendMessageW(toolbar_, TB_ADDBUTTONS, typeCount, reinterpret_cast<LPARAM>(buttons.data()))) return false;
    SendMessageW(toolbar_, TB_AUTOSIZE, 0, 0);
    tooltip_ = reinterpret_cast<HWND>(SendMessageW(toolbar_, TB_GETTOOLTIPS, 0, 0));
    UpdateToolChecks(MyEditor::Instance().CurrentTool());
    return tooltip_ != nullptr;
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    MainWindow* self = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lParam);
        self = static_cast<MainWindow*>(create->lpCreateParams);
        if (!self) return FALSE;
        self->hwnd_ = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    if (!self) return DefWindowProcW(hwnd, message, wParam, lParam);
    const LRESULT result = self->Dispatch(message, wParam, lParam);
    if (message == WM_NCDESTROY) {
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        self->hwnd_ = nullptr;
    }
    return result;
}

LRESULT MainWindow::Dispatch(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: return OnCreate();
    case WM_PAINT: return OnPaint();
    case WM_SIZE: return OnSize(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
    case WM_LBUTTONDOWN: return OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    case WM_MOUSEMOVE: return OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    case WM_LBUTTONUP: return OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    case WM_COMMAND: return OnCommand(LOWORD(wParam));
    case WM_NOTIFY: {
        bool handled = false;
        const LRESULT result = OnNotify(reinterpret_cast<NMHDR*>(lParam), handled);
        return handled ? result : DefWindowProcW(hwnd_, message, wParam, lParam);
    }
    case WM_INITMENUPOPUP:
        return OnInitMenuPopup(reinterpret_cast<HMENU>(wParam), LOWORD(lParam), HIWORD(lParam) != 0);
    case WM_CANCELMODE: return OnCancelMode();
    case WM_CAPTURECHANGED: return OnCaptureChanged(reinterpret_cast<HWND>(lParam));
    case WM_DESTROY: return OnDestroy();
    default: return DefWindowProcW(hwnd_, message, wParam, lParam);
    }
}

LRESULT MainWindow::OnCreate() {
    table_.SetCallback(&MainWindow::TableEvent, this);
    MyEditor::Instance().SetTool(kFirstToolId);
    UpdateToolChecks(kFirstToolId);
    if (CreateToolbar()) return 0;
    if (toolbar_) {
        SendMessageW(toolbar_, TB_SETIMAGELIST, 0, 0);
        DestroyWindow(toolbar_);
        toolbar_ = nullptr;
    }
    if (imageList_) {
        ImageList_Destroy(imageList_);
        imageList_ = nullptr;
    }
    return -1;
}

LRESULT MainWindow::OnPaint() {
    PAINTSTRUCT paint{};
    HDC dc = BeginPaint(hwnd_, &paint);
    if (dc) {
        const int saved = SaveDC(dc);
        RECT client{};
        GetClientRect(hwnd_, &client);
        client.top = toolbarHeight_;
        IntersectClipRect(dc, client.left, client.top, client.right, client.bottom);
        MyEditor::Instance().Show(dc);
        if (saved) RestoreDC(dc, saved);
    }
    EndPaint(hwnd_, &paint);
    return 0;
}

LRESULT MainWindow::OnSize(UINT, int width, int) {
    if (toolbar_) {
        SendMessageW(toolbar_, TB_AUTOSIZE, 0, 0);
        RECT toolbarRect{};
        GetWindowRect(toolbar_, &toolbarRect);
        MapWindowPoints(nullptr, hwnd_, reinterpret_cast<POINT*>(&toolbarRect), 2);
        toolbarHeight_ = toolbarRect.bottom - toolbarRect.top;
        SetWindowPos(toolbar_, nullptr, 0, 0, width, toolbarHeight_,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
    InvalidateRect(hwnd_, nullptr, TRUE);
    return 0;
}

LRESULT MainWindow::OnLButtonDown(int x, int y) {
    if (y < toolbarHeight_) return 0;
    if (!MyEditor::Instance().Begin(x, y)) {
        const wchar_t* text = MyEditor::Instance().Count() >= MyEditor::kCapacity
            ? L"\u0414\u043e\u0441\u044f\u0433\u043d\u0443\u0442\u043e \u043c\u0435\u0436\u0456 128 \u043e\u0431\u0027\u0454\u043a\u0442\u0456\u0432."
            : L"\u041d\u0435 \u0432\u0434\u0430\u043b\u043e\u0441\u044f \u0441\u0442\u0432\u043e\u0440\u0438\u0442\u0438 \u0444\u0456\u0433\u0443\u0440\u0443.";
        MessageBoxW(hwnd_, text, L"\u0420\u0435\u0434\u0430\u043a\u0442\u043e\u0440 \u0444\u0456\u0433\u0443\u0440", MB_OK | MB_ICONINFORMATION);
        return 0;
    }
    SetCapture(hwnd_);
    InvalidateRect(hwnd_, nullptr, TRUE);
    return 0;
}

LRESULT MainWindow::OnMouseMove(WPARAM, int x, int y) {
    if (MyEditor::Instance().HasDraft()) {
        MyEditor::Instance().Update(x, y);
        InvalidateRect(hwnd_, nullptr, TRUE);
    }
    return 0;
}

LRESULT MainWindow::OnLButtonUp(int x, int y) {
    if (MyEditor::Instance().HasDraft()) {
        MyEditor::Instance().Update(x, y);
        MyEditor::Instance().Commit();
        RefreshTable();
        if (GetCapture() == hwnd_) ReleaseCapture();
        InvalidateRect(hwnd_, nullptr, TRUE);
    }
    return 0;
}

LRESULT MainWindow::OnCommand(int commandId) {
    if (FindShapeType(commandId)) {
        CancelInput();
        if (MyEditor::Instance().SetTool(commandId)) {
            UpdateToolChecks(commandId);
            SetWindowTextW(hwnd_, kWindowTitle);
        }
    } else if (commandId == IDM_TABLE) {
        if (table_.IsOpen()) table_.Close();
        else if (table_.Open(instance_, hwnd_)) RefreshTable();
        else MessageBoxW(hwnd_, L"Не вдалося відкрити таблицю.", L"Помилка", MB_OK | MB_ICONERROR);
        CheckMenuItem(GetMenu(hwnd_), IDM_TABLE,
            MF_BYCOMMAND | (table_.IsOpen() ? MF_CHECKED : MF_UNCHECKED));
    } else if (commandId == IDM_SAVE || commandId == IDM_OPEN) {
        CancelInput();
        FileCommand(commandId == IDM_SAVE);
    } else if (commandId == 1) {
        DestroyWindow(hwnd_);
    } else if (commandId == 2) {
        MessageBoxW(hwnd_, L"\u0413\u0440\u0430\u0444\u0456\u0447\u043d\u0438\u0439 \u0440\u0435\u0434\u0430\u043a\u0442\u043e\u0440. \u0412\u0430\u0440\u0456\u0430\u043d\u0442 27.",
            L"\u041b\u0430\u0431\u043e\u0440\u0430\u0442\u043e\u0440\u043d\u0430 \u0440\u043e\u0431\u043e\u0442\u0430 5", MB_OK | MB_ICONINFORMATION);
    }
    return 0;
}

LRESULT MainWindow::OnNotify(NMHDR* notification, bool& handled) {
    handled = false;
    if (!notification || notification->hwndFrom != tooltip_ || notification->code != TTN_GETDISPINFOW) return 0;

    auto* info = reinterpret_cast<NMTTDISPINFOW*>(notification);
    const UINT commandId = static_cast<UINT>(notification->idFrom);
    const auto* type = FindShapeType(commandId);
    if (!type) return 0;
    info->lpszText = const_cast<wchar_t*>(type->name);
    info->uFlags |= TTF_DI_SETITEM;
    handled = true;
    return 0;
}

LRESULT MainWindow::OnInitMenuPopup(HMENU menu, UINT, BOOL isWindowMenu) {
    if (!isWindowMenu && menu && GetMenuState(menu, static_cast<UINT>(MyEditor::Instance().CurrentTool()), MF_BYCOMMAND) != static_cast<UINT>(-1)) {
        UpdateToolChecks(MyEditor::Instance().CurrentTool());
    }
    return 0;
}

LRESULT MainWindow::OnCancelMode() {
    CancelInput();
    return 0;
}

LRESULT MainWindow::OnCaptureChanged(HWND) {
    if (MyEditor::Instance().Cancel()) InvalidateRect(hwnd_, nullptr, TRUE);
    return 0;
}

void MainWindow::CancelInput() {
    MyEditor::Instance().Cancel();
    if (GetCapture() == hwnd_) ReleaseCapture();
    InvalidateRect(hwnd_, nullptr, TRUE);
}

void MainWindow::UpdateToolChecks(int commandId) {
    int count = 0;
    const auto* types = ShapeTypes(count);
    HMENU menu = GetSubMenu(GetMenu(hwnd_), 1);
    for (int i = 0; i < count; ++i) {
        int id = types[i].id;
        if (toolbar_) SendMessageW(toolbar_, TB_CHECKBUTTON, id, MAKELONG(id == commandId, 0));
        if (menu) CheckMenuItem(menu, id, MF_BYCOMMAND | (id == commandId ? MF_CHECKED : MF_UNCHECKED));
    }
}

LRESULT MainWindow::OnDestroy() {
    table_.Close();
    table_.SetCallback(nullptr, nullptr);
    MyEditor::Instance().Clear();
    if (toolbar_ && imageList_) SendMessageW(toolbar_, TB_SETIMAGELIST, 0, 0);
    if (imageList_) {
        ImageList_Destroy(imageList_);
        imageList_ = nullptr;
    }
    toolbar_ = nullptr;
    tooltip_ = nullptr;
    PostQuitMessage(0);
    return 0;
}

void MainWindow::RefreshTable() {
    if (!table_.IsOpen()) return;
    const auto& editor = MyEditor::Instance();
    std::vector<TableRow> rows;
    for (int i = 0; i < editor.Count(); ++i) {
        const Shape& shape = *editor.At(i);
        const auto* type = FindShapeType(shape.TypeId());
        rows.push_back({type ? type->name : L"Невідомий", shape.StartX(), shape.StartY(), shape.EndX(), shape.EndY()});
    }
    table_.SetRows(rows, editor.Selected());
}

void MainWindow::TableEvent(void* context, MyTable::Event event, int row) {
    auto& self = *static_cast<MainWindow*>(context);
    auto& editor = MyEditor::Instance();
    if (event == MyTable::Event::Select) editor.Select(row);
    else if (event == MyTable::Event::Delete) {
        self.CancelInput();
        editor.Delete(row);
        self.RefreshTable();
    } else {
        editor.Select(-1);
        CheckMenuItem(GetMenu(self.hwnd_), IDM_TABLE, MF_BYCOMMAND | MF_UNCHECKED);
    }
    InvalidateRect(self.hwnd_, nullptr, TRUE);
}

void MainWindow::FileCommand(bool save) {
    wchar_t path[MAX_PATH]{};
    OPENFILENAMEW dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = hwnd_;
    dialog.lpstrFilter = L"Фігури Lab5 (*.txt)\0*.txt\0Усі файли (*.*)\0*.*\0\0";
    dialog.lpstrFile = path;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrDefExt = L"txt";
    dialog.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR
        | (save ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);
    const BOOL chosen = save ? GetSaveFileNameW(&dialog) : GetOpenFileNameW(&dialog);
    if (!chosen) {
        if (CommDlgExtendedError()) MessageBoxW(hwnd_, L"Помилка діалогу вибору файлу.", L"Помилка", MB_OK | MB_ICONERROR);
        return;
    }
    if (!save && MyEditor::Instance().Count() > 0
        && MessageBoxW(hwnd_, L"Замінити поточний малюнок даними з файлу?", L"Відкрити файл",
            MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES) return;
    std::wstring error;
    const bool ok = save ? MyEditor::Instance().Save(path, error) : MyEditor::Instance().Load(path, error);
    if (!ok) MessageBoxW(hwnd_, error.c_str(), L"Помилка файлу", MB_OK | MB_ICONERROR);
    else {
        if (!save) RefreshTable();
        InvalidateRect(hwnd_, nullptr, TRUE);
        SetWindowTextW(hwnd_, (std::wstring(kWindowTitle) + L" — " + path).c_str());
    }
}
