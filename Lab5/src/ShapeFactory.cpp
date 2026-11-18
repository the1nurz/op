#include "ShapeFactory.h"
#include "PointShape.h"
#include "LineShape.h"
#include "RectShape.h"
#include "EllipseShape.h"
#include "CircleLineShape.h"
#include "CubeShape.h"
#include "../resources/resource.h"
#include <new>

namespace {
template<class T> Shape* Make(int x, int y) { return new (std::nothrow) T(x, y); }
const ShapeType types[] = {
    {10, L"Крапка", IDB_TOOL_DOT, Make<PointShape>},
    {11, L"Лінія", IDB_TOOL_LINE, Make<LineShape>},
    {12, L"Прямокутник", IDB_TOOL_RECT, Make<RectShape>},
    {13, L"Еліпс", IDB_TOOL_ELLIPSE, Make<EllipseShape>},
    {14, L"Лінія з кружечками", IDB_TOOL_CIRCLE_LINE, Make<CircleLineShape>},
    {15, L"Каркас куба", IDB_TOOL_CUBE, Make<CubeShape>}
};
}
const ShapeType* ShapeTypes(int& count) noexcept {
    count = static_cast<int>(sizeof(types)/sizeof(types[0]));
    return types;
}
const ShapeType* FindShapeType(int id) noexcept {
    for (const auto& type : types) if (type.id == id) return &type;
    return nullptr;
}
Shape* CreateShapeByType(int id, int x, int y) noexcept {
    const auto* type = FindShapeType(id);
    return type ? type->create(x, y) : nullptr;
}
