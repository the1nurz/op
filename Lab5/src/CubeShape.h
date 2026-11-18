#pragma once
#include "LineShape.h"
#include "RectShape.h"

class CubeShape : public LineShape, public RectShape {
public:
    CubeShape(int x, int y) : Shape(x, y), LineShape(x, y), RectShape(x, y) {}
    RECT Bounds() const noexcept override;
    int TypeId() const noexcept override { return 15; }
    void Show(HDC dc, bool preview = false) const override;
};
