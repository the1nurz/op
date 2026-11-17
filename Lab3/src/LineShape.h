#pragma once
#include "Shape.h"
class LineShape : public Shape {
public:
    LineShape(int x, int y) : Shape(x,y) {}
    void Draw(HDC dc, bool preview = false) const override;
};
