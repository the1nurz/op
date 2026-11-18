#pragma once
#include <windows.h>
#include <string>
#include <vector>

// Таблиця отримує готові значення, не залежить від Shape, MyEditor чи MainWindow.
struct TableRow {
    std::wstring type;
    int x1, y1, x2, y2;
};

class MyTable {
public:
    enum class Event { Select, Delete, Closed };
    using Callback = void (*)(void* context, Event event, int row);
    MyTable() = default;
    ~MyTable() { Close(); }
    MyTable(const MyTable&) = delete;
    MyTable& operator=(const MyTable&) = delete;

    void SetCallback(Callback callback, void* context) noexcept;
    bool Open(HINSTANCE instance, HWND owner);
    void Close();
    bool IsOpen() const noexcept { return hwnd_ != nullptr; }
    bool ProcessMessage(MSG& message) const;
    void SetRows(const std::vector<TableRow>& rows, int selected);

private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    INT_PTR Dispatch(UINT message, WPARAM wParam, LPARAM lParam);
    void Notify(Event event, int row);
    void Resize();
    HWND hwnd_ = nullptr;
    HWND list_ = nullptr;
    bool updating_ = false;
    Callback callback_ = nullptr;
    void* context_ = nullptr;
};
