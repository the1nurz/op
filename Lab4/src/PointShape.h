#pragma once
#include "Shape.h"
class PointShape : public Shape {
public:
    PointShape(int x, int y) : Shape(x,y) {}
    void Show(HDC dc, bool preview = false) const override;
};
