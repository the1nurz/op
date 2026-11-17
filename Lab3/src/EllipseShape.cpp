#include "EllipseShape.h"
#include "GdiGuards.h"
#include <algorithm>
#include <cstdlib>

void EllipseShape::Draw(HDC dc, bool) const {
    if (!dc) return;
    ScopedGdiObject pen(CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
    if (!pen.Get()) return;
    ScopedGdiSelection selectedPen(dc, pen.Get());
    if (!selectedPen.IsSelected()) return;

    ScopedGdiObject brush(CreateSolidBrush(RGB(255, 192, 203)));
    if (!brush.Get()) return;
    ScopedGdiSelection selectedBrush(dc, brush.Get());
    if (!selectedBrush.IsSelected()) return;

    const int centerX = StartX();
    const int centerY = StartY();
    const int radiusX = (std::max)(std::abs(EndX() - centerX), 1);
    const int radiusY = (std::max)(std::abs(EndY() - centerY), 1);
    const int left = centerX - radiusX;
    const int top = centerY - radiusY;
    const int right = centerX + radiusX;
    const int bottom = centerY + radiusY;
    ::Ellipse(dc, left, top, right, bottom);
}
