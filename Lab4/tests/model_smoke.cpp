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
#include "CircleLineShape.h"
#include "CubeShape.h"
#include <vector>

static_assert(std::is_abstract<Shape>::value, "Shape must remain abstract");
static_assert(std::has_virtual_destructor<Shape>::value, "Deletion through Shape must be safe");
static_assert(std::is_convertible<CircleLineShape*, Shape*>::value, "One shared Shape base");
static_assert(std::is_convertible<CubeShape*, Shape*>::value, "One shared Shape base");
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
    point.Show(dc);
    int blackPointPixels = 0;
    for (int y = 27; y <= 33; ++y) for (int x = 17; x <= 23; ++x)
        if (GetPixel(dc, x, y) == RGB(0, 0, 0)) ++blackPointPixels;
    assert(blackPointPixels > 0);

    FillRect(dc, &canvas, white);
    line.Show(dc);
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
        rectangle.Show(dc);
        assert(GetPixel(dc, 75, 80) == RGB(255, 255, 255));
        assert(GetPixel(dc, 75, 60) == RGB(0, 0, 0));
    }

    const POINT ellipseCorners[4] = {{220, 180}, {180, 180}, {220, 220}, {180, 220}};
    for (const POINT corner : ellipseCorners) {
        EllipseShape ellipse(200, 200);
        ellipse.SetEnd(corner.x, corner.y);
        FillRect(dc, &canvas, white);
        ellipse.Show(dc);
        assert(GetPixel(dc, 200, 200) == RGB(255, 255, 255));
        int blackOutlinePixels = 0;
        for (int y = 169; y <= 231; ++y) for (int x = 169; x <= 231; ++x)
            if (GetPixel(dc, x, y) == RGB(0, 0, 0)) ++blackOutlinePixels;
        assert(blackOutlinePixels > 0);
    }

    CircleLineShape circleLine(40, 120);
    circleLine.SetEnd(220, 120);
    CubeShape cube(50, 180);
    cube.SetEnd(140, 270);
    FillRect(dc, &canvas, white);
    circleLine.Show(dc);
    assert(GetPixel(dc, 100, 120) == RGB(0, 0, 0));
    assert(GetPixel(dc, 34, 120) == RGB(0, 0, 0));
    assert(GetPixel(dc, 226 - 1, 120) == RGB(0, 0, 0));
    FillRect(dc, &canvas, white);
    cube.Show(dc);
    assert(GetPixel(dc, 90, 180) == RGB(0, 0, 0));
    assert(GetPixel(dc, 100, 150) == RGB(0, 0, 0));
    assert(GetPixel(dc, 65, 165) == RGB(0, 0, 0));

    RectShape rectangle(30, 40);
    rectangle.SetEnd(240, 140);
    EllipseShape ellipse(150, 150);
    ellipse.SetEnd(240, 210);
    LineShape longLine(20, 80);
    longLine.SetEnd(280, 80);
    Shape* shapes[] = {&point, &longLine, &rectangle, &ellipse, &circleLine, &cube};
    HGDIOBJ originalPen = GetCurrentObject(dc, OBJ_PEN);
    HGDIOBJ originalBrush = GetCurrentObject(dc, OBJ_BRUSH);
    const int originalMode = GetBkMode(dc);
    for (Shape* shape : shapes) {
        int count[2]{};
        std::vector<COLORREF> firstPaint;
        for (int preview = 0; preview < 2; ++preview) {
            FillRect(dc, &canvas, white);
            shape->Show(dc, preview != 0);
            for (int y = 0; y < 320; ++y) {
                for (int x = 0; x < 320; ++x) {
                    const COLORREF color = GetPixel(dc, x, y);
                    if (color == RGB(0, 0, 0)) ++count[preview];
                    if (!preview) firstPaint.push_back(color);
                }
            }
            assert(GetCurrentObject(dc, OBJ_PEN) == originalPen);
            assert(GetCurrentObject(dc, OBJ_BRUSH) == originalBrush);
            assert(GetBkMode(dc) == originalMode);
        }
        assert(count[0] > count[1] && count[1] > 0);
        FillRect(dc, &canvas, white);
        shape->Show(dc);
        int index = 0;
        for (int y = 0; y < 320; ++y) for (int x = 0; x < 320; ++x)
            assert(GetPixel(dc, x, y) == firstPaint[index++]);
    }

    const DWORD gdiBefore = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    for (int i = 0; i < 500; ++i)
        for (Shape* shape : shapes) shape->Show(dc, i % 2 != 0);
    assert(GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS) == gdiBefore);

    Shape* shared = &circleLine;
    assert(static_cast<Shape*>(static_cast<LineShape*>(&circleLine)) == shared);
    assert(static_cast<Shape*>(static_cast<EllipseShape*>(&circleLine)) == shared);

    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
    return 0;
}
