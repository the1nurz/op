#include "LineShape.h"
#include "GdiGuards.h"

void LineShape::Draw(HDC dc, bool) const {
    if (!dc) return;
    ScopedGdiObject pen(CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
    if (!pen.Get()) return;
    ScopedGdiSelection selectedPen(dc, pen.Get());
    if (!selectedPen.IsSelected()) return;
    MoveToEx(dc, StartX(), StartY(), nullptr);
    LineTo(dc, EndX(), EndY());
}
