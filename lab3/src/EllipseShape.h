#pragma once
#include "Shape.h"
class EllipseShape : public Shape {
public:
    EllipseShape(int x, int y) : Shape(x,y) {}
    void Draw(HDC dc, bool preview = false) const override;
};
