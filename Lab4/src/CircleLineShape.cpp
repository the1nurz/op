#include "CircleLineShape.h"

void CircleLineShape::Show(HDC dc, bool preview) const {
    const int radius = 6;
    LineShape::Show(dc, preview);
    // Кружечки задаються центром і кутом рамки.
    EllipseShape::Show(dc, StartX(), StartY(), StartX() + radius, StartY() + radius, preview);
    EllipseShape::Show(dc, EndX(), EndY(), EndX() + radius, EndY() + radius, preview);
}
