#pragma once
#include <windows.h>

class ScopedGdiSelection {
public:
    ScopedGdiSelection(HDC dc, HGDIOBJ object) noexcept : dc_(dc), old_(nullptr) {
        if (dc_ && object) {
            old_ = SelectObject(dc_, object);
            if (old_ == HGDI_ERROR) old_ = nullptr;
        }
    }
    ~ScopedGdiSelection() { if (old_) SelectObject(dc_, old_); }
    ScopedGdiSelection(const ScopedGdiSelection&) = delete;
    ScopedGdiSelection& operator=(const ScopedGdiSelection&) = delete;
    bool IsSelected() const noexcept { return old_ != nullptr; }
private:
    HDC dc_;
    HGDIOBJ old_;
};

class ScopedGdiObject {
public:
    explicit ScopedGdiObject(HGDIOBJ object = nullptr) noexcept : object_(object) {}
    ~ScopedGdiObject() { if (object_) DeleteObject(object_); }
    ScopedGdiObject(const ScopedGdiObject&) = delete;
    ScopedGdiObject& operator=(const ScopedGdiObject&) = delete;
    HGDIOBJ Get() const noexcept { return object_; }
private:
    HGDIOBJ object_;
};
