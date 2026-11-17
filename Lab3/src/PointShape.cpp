#include "PointShape.h"
#include "GdiGuards.h"

void PointShape::Draw(HDC dc, bool) const {
    if (!dc) return;
    ScopedGdiObject pen(CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
    if (!pen.Get()) return;
    ScopedGdiSelection selectedPen(dc, pen.Get());
    if (!selectedPen.IsSelected()) return;
    MoveToEx(dc, StartX() - 3, StartY(), nullptr);
    LineTo(dc, StartX() + 4, StartY());
    MoveToEx(dc, StartX(), StartY() - 3, nullptr);
    LineTo(dc, StartX(), StartY() + 4);
}
