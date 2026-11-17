#include "EllipseShape.h"
#include "GdiGuards.h"
#include <algorithm>
#include <cstdlib>

void EllipseShape::Show(HDC dc, bool preview) const {
    Show(dc, StartX(), StartY(), EndX(), EndY(), preview);
}

void EllipseShape::Show(HDC dc, int x1, int y1, int x2, int y2, bool preview) const {
    if (!dc) return;
    ScopedGdiObject pen(CreatePen(preview ? PS_DOT : PS_SOLID, 1, RGB(0, 0, 0)));
    if (!pen.Get()) return;
    ScopedGdiSelection selectedPen(dc, pen.Get());
    if (!selectedPen.IsSelected()) return;

    HBRUSH brush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    if (!brush) return;
    ScopedGdiSelection selectedBrush(dc, brush);
    if (!selectedBrush.IsSelected()) return;

    const int oldMode = SetBkMode(dc, TRANSPARENT);
    const int centerX = x1;
    const int centerY = y1;
    const int radiusX = (std::max)(std::abs(x2 - centerX), 1);
    const int radiusY = (std::max)(std::abs(y2 - centerY), 1);
    const int left = centerX - radiusX;
    const int top = centerY - radiusY;
    const int right = centerX + radiusX;
    const int bottom = centerY + radiusY;
    ::Ellipse(dc, left, top, right, bottom);
    SetBkMode(dc, oldMode);
}
