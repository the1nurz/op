#include "CubeShape.h"
#include <algorithm>
#include <cstdlib>

void CubeShape::Show(HDC dc, bool preview) const {
    // Передня грань квадратна, задня зміщена вправо та вгору.
    const int side = (std::max)(1, (std::min)(
        std::abs(EndX() - StartX()), std::abs(EndY() - StartY())));
    const int x1 = StartX();
    const int y1 = StartY();
    const int x2 = x1 + (EndX() < x1 ? -side : side);
    const int y2 = y1 + (EndY() < y1 ? -side : side);
    const int offset = (std::max)(1, side / 3);

    RectShape::Show(dc, x1, y1, x2, y2, preview);
    RectShape::Show(dc, x1 + offset, y1 - offset, x2 + offset, y2 - offset, preview);
    LineShape::Show(dc, x1, y1, x1 + offset, y1 - offset, preview);
    LineShape::Show(dc, x2, y1, x2 + offset, y1 - offset, preview);
    LineShape::Show(dc, x1, y2, x1 + offset, y2 - offset, preview);
    LineShape::Show(dc, x2, y2, x2 + offset, y2 - offset, preview);
}
