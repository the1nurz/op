#include "EllipseShape.h"
#include <algorithm>
#include <cstdlib>
void EllipseShape::Draw(HDC dc, bool preview) const {
    HPEN pen = CreatePen(preview ? PS_DOT : PS_SOLID, 1, RGB(0,0,0));
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HBRUSH fill = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    HGDIOBJ oldBrush = SelectObject(dc, fill);
    ::Ellipse(dc, std::min(x1,x2), std::min(y1,y2), std::max(x1,x2), std::max(y1,y2));
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);

}
