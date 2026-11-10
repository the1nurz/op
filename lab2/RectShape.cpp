#include "RectShape.h"
#include <algorithm>
#include <cstdlib>
void RectShape::Draw(HDC dc, bool preview) const {
    HPEN pen = CreatePen(preview ? PS_DOT : PS_SOLID, 1, RGB(0,0,0));
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HBRUSH fill = preview ? static_cast<HBRUSH>(GetStockObject(NULL_BRUSH)) : CreateSolidBrush(RGB(255,192,203));
    HGDIOBJ oldBrush = SelectObject(dc, fill);
    int dx = abs(x2-x1), dy = abs(y2-y1);
    Rectangle(dc, x1-dx, y1-dy, x1+dx, y1+dy);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
    if (!preview) DeleteObject(fill);
}
