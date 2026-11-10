#include <windows.h>
#include <windowsx.h>
#include <new>
#include "PointShape.h"
#include "LineShape.h"
#include "RectShape.h"
#include "EllipseShape.h"
namespace {
constexpr int N = 127;
Shape** pcshape = nullptr;
int count = 0;
int tool = 10;
Shape* draft = nullptr;
const wchar_t* names[] = {L"Крапка", L"Лінія", L"Прямокутник", L"Еліпс"};
void Cancel(HWND window) {
    delete draft; draft = nullptr;
    if (GetCapture() == window) ReleaseCapture();
    InvalidateRect(window, nullptr, TRUE);
}
void SelectTool(HWND window, int id) {
    Cancel(window);
    tool = id;
    CheckMenuRadioItem(GetMenu(window), 10, 13, tool, MF_BYCOMMAND);
    wchar_t title[100];
    wsprintfW(title, L"Lab2 - %s", names[tool-10]);
    SetWindowTextW(window, title);
}
Shape* CreateShape(int x, int y) {
    switch (tool) {
    case 10: return new(std::nothrow) PointShape(x,y);
    case 11: return new(std::nothrow) LineShape(x,y);
    case 12: return new(std::nothrow) RectShape(x,y);
    default: return new(std::nothrow) EllipseShape(x,y);
    }
}
HMENU CreateEditorMenu() {
    HMENU menu = CreateMenu(), file = CreatePopupMenu();
    HMENU objects = CreatePopupMenu(), help = CreatePopupMenu();
    AppendMenuW(file, MF_STRING, 1, L"Вихід");
    for (int i=0; i<4; ++i) AppendMenuW(objects, MF_STRING, 10+i, names[i]);
    AppendMenuW(help, MF_STRING, 2, L"Про програму");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(file), L"Файл");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(objects), L"Об'єкти");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(help), L"Довідка");
    return menu;
}
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    switch (message) {
    case WM_CREATE: SelectTool(window,10); return 0;
    case WM_COMMAND:
        if (LOWORD(wp)>=10 && LOWORD(wp)<=13) SelectTool(window,LOWORD(wp));
        else if (LOWORD(wp)==1) DestroyWindow(window);
        else if (LOWORD(wp)==2) MessageBoxW(window,
            L"Графічний редактор. Варіант 27.\nОберіть фігуру в меню Об'єкти.\nНатисніть і перетягніть мишу. Esc скасовує введення.\nПрямокутник: від центра до кута. Максимум 127 фігур.",
            L"Лабораторна робота 2", MB_OK);
        return 0;
    case WM_LBUTTONDOWN:
        if (draft) return 0;
        if (count == N) {
            MessageBoxW(window,L"Досягнуто межі 127 об'єктів.",L"Масив заповнено",MB_OK);
            return 0;
        }
        draft = CreateShape(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
        if (!draft) { MessageBoxW(window,L"Недостатньо пам'яті.",L"Помилка",MB_OK); return 0; }
        SetCapture(window);
        InvalidateRect(window,nullptr,TRUE);
        return 0;
    case WM_MOUSEMOVE:
        if (draft) {
            draft->SetEnd(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
            InvalidateRect(window,nullptr,TRUE);
        }
        return 0;
    case WM_LBUTTONUP:
        if (draft) {
            draft->SetEnd(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
            pcshape[count++] = draft;
            draft = nullptr;
            ReleaseCapture();
            InvalidateRect(window,nullptr,TRUE);
        }
        return 0;
    case WM_KEYDOWN: if (wp==VK_ESCAPE) Cancel(window); return 0;
    case WM_CANCELMODE: Cancel(window); return 0;
    case WM_CAPTURECHANGED:
        if (draft) Cancel(window);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(window,&ps);
        // Збережені об'єкти малюються через базовий вказівник.
        for (int i=0; i<count; ++i) pcshape[i]->Draw(dc);
        if (draft) draft->Draw(dc,true);
        EndPaint(window,&ps);
        return 0;
    }
    case WM_DESTROY:
        Cancel(window);
        for (int i=0; i<count; ++i) delete pcshape[i];
        delete[] pcshape; pcshape=nullptr;
        PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(window,message,wp,lp);
}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR,int show) {
    pcshape = new(std::nothrow) Shape*[N]{};
    if (!pcshape) return 1;
    WNDCLASSW wc{};
    wc.lpfnWndProc=WindowProc; wc.hInstance=instance;
    wc.lpszClassName=L"Lab2ShapeEditor";
    wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
    wc.hbrBackground=static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    if (!RegisterClassW(&wc)) { delete[] pcshape; return 1; }
    HMENU menu=CreateEditorMenu();
    HWND window=CreateWindowW(wc.lpszClassName,L"Lab2",WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,CW_USEDEFAULT,960,700,nullptr,menu,instance,nullptr);
    if (!window) { DestroyMenu(menu); delete[] pcshape; return 1; }
    ShowWindow(window,show);
    MSG msg;
    int result;
    while ((result=GetMessageW(&msg,nullptr,0,0))>0) {
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
    if (result==-1) { if (IsWindow(window)) DestroyWindow(window); return 1; }
    return static_cast<int>(msg.wParam);
}
