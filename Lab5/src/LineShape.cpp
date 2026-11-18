#include "LineShape.h"
#include "GdiGuards.h"

void LineShape::Show(HDC dc, bool preview) const {
    Show(dc, StartX(), StartY(), EndX(), EndY(), preview);
}

void LineShape::Show(HDC dc, int x1, int y1, int x2, int y2, bool preview) const {
    if (!dc) return;
    ScopedGdiObject pen(CreatePen(preview ? PS_DOT : PS_SOLID, 1, RGB(0, 0, 0)));
    if (!pen.Get()) return;
    ScopedGdiSelection selectedPen(dc, pen.Get());
    if (!selectedPen.IsSelected()) return;
    const int oldMode = SetBkMode(dc, TRANSPARENT);
    MoveToEx(dc, x1, y1, nullptr);
    LineTo(dc, x2, y2);
    SetBkMode(dc, oldMode);
}
