#pragma once
#include "Shape.h"
class LineShape : virtual public Shape {
public:
    LineShape(int x, int y) : Shape(x,y) {}
    void Show(HDC dc, bool preview = false) const override;
protected:
    void Show(HDC dc, int x1, int y1, int x2, int y2, bool preview) const;
};
