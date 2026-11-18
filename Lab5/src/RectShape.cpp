#include "RectShape.h"
#include "GdiGuards.h"
#include <algorithm>

void RectShape::Show(HDC dc, bool preview) const {
    Show(dc, StartX(), StartY(), EndX(), EndY(), preview);
}

void RectShape::Show(HDC dc, int x1, int y1, int x2, int y2, bool preview) const {
    if (!dc) return;
    ScopedGdiObject pen(CreatePen(preview ? PS_DOT : PS_SOLID, 1, RGB(0, 0, 0)));
    if (!pen.Get()) return;
    ScopedGdiSelection selectedPen(dc, pen.Get());
    if (!selectedPen.IsSelected()) return;

    HBRUSH nullBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    if (!nullBrush) return;
    ScopedGdiSelection selectedBrush(dc, nullBrush);
    if (!selectedBrush.IsSelected()) return;

    const int oldMode = SetBkMode(dc, TRANSPARENT);
    const int left = (std::min)(x1, x2);
    const int top = (std::min)(y1, y2);
    const int right = (std::max)(x1, x2);
    const int bottom = (std::max)(y1, y2);
    Rectangle(dc, left, top, right, bottom);
    SetBkMode(dc, oldMode);
}
