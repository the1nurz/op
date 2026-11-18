#pragma once

#include <windows.h>
#include <commctrl.h>
#include "MyEditor.h"
#include "MyTable.h"

class MainWindow {
public:
    MainWindow() noexcept = default;
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

    bool Create(HINSTANCE instance, int showCommand);
    int Run();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT Dispatch(UINT message, WPARAM wParam, LPARAM lParam);

    LRESULT OnCreate();
    LRESULT OnPaint();
    LRESULT OnSize(UINT state, int width, int height);
    LRESULT OnLButtonDown(int x, int y);
    LRESULT OnMouseMove(WPARAM keyFlags, int x, int y);
    LRESULT OnLButtonUp(int x, int y);
    LRESULT OnCommand(int commandId);
    LRESULT OnNotify(NMHDR* notification, bool& handled);
    LRESULT OnInitMenuPopup(HMENU menu, UINT position, BOOL isWindowMenu);
    LRESULT OnCancelMode();
    LRESULT OnCaptureChanged(HWND newCapture);
    LRESULT OnDestroy();

    bool CreateToolbar();
    void UpdateToolChecks(int commandId);
    void CancelInput();
    HMENU CreateMenuBar() const;

    static void TableEvent(void* context, MyTable::Event event, int row);
    void RefreshTable();
    void FileCommand(bool save);
    MyTable table_;
    HWND hwnd_ = nullptr;
    HWND toolbar_ = nullptr;
    HWND tooltip_ = nullptr;
    HIMAGELIST imageList_ = nullptr;
    int toolbarHeight_ = 0;
    HINSTANCE instance_ = nullptr;
};
