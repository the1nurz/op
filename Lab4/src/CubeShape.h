#pragma once
#include "LineShape.h"
#include "RectShape.h"

class CubeShape : public LineShape, public RectShape {
public:
    CubeShape(int x, int y) : Shape(x, y), LineShape(x, y), RectShape(x, y) {}
    void Show(HDC dc, bool preview = false) const override;
};
