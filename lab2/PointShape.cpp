#include "PointShape.h"
#include <algorithm>
#include <cstdlib>
void PointShape::Draw(HDC dc, bool preview) const {
    HPEN pen = CreatePen(preview ? PS_DOT : PS_SOLID, 1, RGB(0,0,0));
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HBRUSH fill = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    HGDIOBJ oldBrush = SelectObject(dc, fill);
    MoveToEx(dc, x1-3, y1, nullptr); LineTo(dc, x1+4, y1);
    MoveToEx(dc, x1, y1-3, nullptr); LineTo(dc, x1, y1+4);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);

}
