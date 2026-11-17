#include <windows.h>
#include <cassert>
#include <type_traits>
#include <algorithm>
#include <cstdlib>
#include "Shape.h"
#include "PointShape.h"
#include "LineShape.h"
#include "RectShape.h"
#include "EllipseShape.h"

static_assert(std::is_abstract<Shape>::value, "Shape must remain abstract");
static_assert(std::extent<Shape*[128]>::value == 128, "Variant capacity must be 128");

int main() {
    PointShape point(20, 30);
    LineShape line(10, 15);
    point.SetEnd(25, 35);
    line.SetEnd(30, 40);
    assert(point.StartX() == 20 && point.StartY() == 30);
    assert(line.EndX() == 30 && line.EndY() == 40);

    HDC screenDc = GetDC(nullptr);
    assert(screenDc);
    HDC dc = CreateCompatibleDC(screenDc);
    assert(dc);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDc, 320, 320);
    ReleaseDC(nullptr, screenDc);
    assert(bitmap);
    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
    assert(oldBitmap && oldBitmap != HGDI_ERROR);
    RECT canvas{0, 0, 320, 320};
    HBRUSH white = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));

    FillRect(dc, &canvas, white);
    point.Draw(dc);
    int blackPointPixels = 0;
    for (int y = 27; y <= 33; ++y) for (int x = 17; x <= 23; ++x)
        if (GetPixel(dc, x, y) == RGB(0, 0, 0)) ++blackPointPixels;
    assert(blackPointPixels > 0);

    FillRect(dc, &canvas, white);
    line.Draw(dc);
    int blackLinePixels = 0;
    for (int y = 15; y <= 40; ++y) for (int x = 10; x <= 30; ++x)
        if (GetPixel(dc, x, y) == RGB(0, 0, 0)) ++blackLinePixels;
    assert(blackLinePixels > 0);

    const POINT rectangleCorners[4][2] = {
        {{100, 100}, {50, 60}}, {{50, 100}, {100, 60}},
        {{100, 60}, {50, 100}}, {{50, 60}, {100, 100}}
    };
    for (const auto& corners : rectangleCorners) {
        RectShape rectangle(corners[0].x, corners[0].y);
        rectangle.SetEnd(corners[1].x, corners[1].y);
        FillRect(dc, &canvas, white);
        rectangle.Draw(dc);
        assert(GetPixel(dc, 75, 80) == RGB(255, 255, 255));
        assert(GetPixel(dc, 75, 60) == RGB(0, 0, 0));
    }

    const POINT ellipseCorners[4] = {{220, 180}, {180, 180}, {220, 220}, {180, 220}};
    for (const POINT corner : ellipseCorners) {
        EllipseShape ellipse(200, 200);
        ellipse.SetEnd(corner.x, corner.y);
        FillRect(dc, &canvas, white);
        ellipse.Draw(dc);
        assert(GetPixel(dc, 200, 200) == RGB(255, 192, 203));
        int blackOutlinePixels = 0;
        for (int y = 169; y <= 231; ++y) for (int x = 169; x <= 231; ++x)
            if (GetPixel(dc, x, y) == RGB(0, 0, 0)) ++blackOutlinePixels;
        assert(blackOutlinePixels > 0);
    }

    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
    return 0;
}
