#pragma once

#include "Shape.h"

class MyEditor {
public:
    static constexpr int kCapacity = 128;

    MyEditor() noexcept = default;
    ~MyEditor();
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

private:
    Shape* CreateShape(int x, int y) const noexcept;

    Shape* pcshape[kCapacity]{};
    int count_ = 0;
    int toolId_ = 10;
    Shape* draft_ = nullptr;
};
