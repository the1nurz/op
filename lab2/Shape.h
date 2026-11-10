#pragma once
#include <windows.h>
class Shape {
protected:
    int x1, y1, x2, y2;
public:
    Shape(int x, int y) : x1(x), y1(y), x2(x), y2(y) {}
    virtual ~Shape();
    void SetEnd(int x, int y);
    virtual void Draw(HDC dc, bool preview = false) const = 0;
};
