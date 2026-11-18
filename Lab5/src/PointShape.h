#pragma once
#include "Shape.h"
class PointShape : public Shape {
public:
    PointShape(int x, int y) : Shape(x,y) {}
    RECT Bounds() const noexcept override;
    int TypeId() const noexcept override { return 10; }
    void Show(HDC dc, bool preview = false) const override;
};
