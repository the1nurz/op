#pragma once
#include "LineShape.h"
#include "EllipseShape.h"

class CircleLineShape : public LineShape, public EllipseShape {
public:
    CircleLineShape(int x, int y) : Shape(x, y), LineShape(x, y), EllipseShape(x, y) {}
    void Show(HDC dc, bool preview = false) const override;
};
