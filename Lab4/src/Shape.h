#pragma once

#include <windows.h>

class Shape {
public:
    Shape(int x, int y) noexcept;
    virtual ~Shape();

    void SetEnd(int x, int y) noexcept;
    int StartX() const noexcept;
    int StartY() const noexcept;
    int EndX() const noexcept;
    int EndY() const noexcept;

    virtual void Show(HDC dc, bool preview = false) const = 0;

private:
    int x1_;
    int y1_;
    int x2_;
    int y2_;
};
