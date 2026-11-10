#pragma once
#include "Shape.h"
class RectShape : public Shape {
public:
    RectShape(int x, int y) : Shape(x,y) {}
    void Draw(HDC dc, bool preview = false) const override;
};
