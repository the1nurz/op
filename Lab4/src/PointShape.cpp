#include "PointShape.h"
#include "GdiGuards.h"

void PointShape::Show(HDC dc, bool preview) const {
    if (!dc) return;
    ScopedGdiObject pen(CreatePen(preview ? PS_DOT : PS_SOLID, 1, RGB(0, 0, 0)));
    if (!pen.Get()) return;
    ScopedGdiSelection selectedPen(dc, pen.Get());
    if (!selectedPen.IsSelected()) return;
    const int oldMode = SetBkMode(dc, TRANSPARENT);
    MoveToEx(dc, StartX() - 3, StartY(), nullptr);
    LineTo(dc, StartX() + 4, StartY());
    MoveToEx(dc, StartX(), StartY() - 3, nullptr);
    LineTo(dc, StartX(), StartY() + 4);
    SetBkMode(dc, oldMode);
}
