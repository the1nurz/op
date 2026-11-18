#pragma once
#include "Shape.h"

// Єдине місце реєстрації типів: редактор і таблиця не знають похідних класів.
struct ShapeType {
    int id;
    const wchar_t* name;
    int bitmapId;
    Shape* (*create)(int, int);
};
const ShapeType* ShapeTypes(int& count) noexcept;
const ShapeType* FindShapeType(int id) noexcept;
Shape* CreateShapeByType(int id, int x, int y) noexcept;
