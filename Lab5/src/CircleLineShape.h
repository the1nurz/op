#pragma once
#include "LineShape.h"
#include "EllipseShape.h"

class CircleLineShape : public LineShape, public EllipseShape {
public:
    CircleLineShape(int x, int y) : Shape(x, y), LineShape(x, y), EllipseShape(x, y) {}
    RECT Bounds() const noexcept override;
    int TypeId() const noexcept override { return 14; }
    void Show(HDC dc, bool preview = false) const override;
};
