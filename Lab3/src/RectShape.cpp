#include "RectShape.h"
#include "GdiGuards.h"
#include <algorithm>

void RectShape::Draw(HDC dc, bool) const {
    if (!dc) return;
    ScopedGdiObject pen(CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
    if (!pen.Get()) return;
    ScopedGdiSelection selectedPen(dc, pen.Get());
    if (!selectedPen.IsSelected()) return;

    HBRUSH nullBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    if (!nullBrush) return;
    ScopedGdiSelection selectedBrush(dc, nullBrush);
    if (!selectedBrush.IsSelected()) return;

    const int left = (std::min)(StartX(), EndX());
    const int top = (std::min)(StartY(), EndY());
    const int right = (std::max)(StartX(), EndX());
    const int bottom = (std::max)(StartY(), EndY());
    Rectangle(dc, left, top, right, bottom);
}
