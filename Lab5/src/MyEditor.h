#pragma once

#include "Shape.h"
#include <string>

class MyEditor {
public:
    static constexpr int kCapacity = 128;

    static MyEditor& Instance() noexcept;
    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;

    bool SetTool(int commandId) noexcept;
    int CurrentTool() const noexcept;
    bool Begin(int x, int y) noexcept;
    void Update(int x, int y) noexcept;
    bool Commit() noexcept;
    bool Cancel() noexcept;
    bool HasDraft() const noexcept;
    int Count() const noexcept;
    void Show(HDC dc) const;
    void Clear() noexcept;
    const Shape* At(int index) const noexcept;
    bool Delete(int index) noexcept;
    void Select(int index) noexcept;
    int Selected() const noexcept { return selected_; }
    bool Save(const std::wstring& path, std::wstring& error) const;
    bool Load(const std::wstring& path, std::wstring& error);

private:
    MyEditor() noexcept = default;
    ~MyEditor();
    Shape* CreateShape(int x, int y) const noexcept;

    Shape* pcshape[kCapacity]{};
    int count_ = 0;
    int toolId_ = 10;
    Shape* draft_ = nullptr;
    int selected_ = -1;
};
