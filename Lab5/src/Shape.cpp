#include "Shape.h"

Shape::Shape(int x, int y) noexcept : x1_(x), y1_(y), x2_(x), y2_(y) {}
Shape::~Shape() = default;

void Shape::SetEnd(int x, int y) noexcept { x2_ = x; y2_ = y; }
int Shape::StartX() const noexcept { return x1_; }
int Shape::StartY() const noexcept { return y1_; }
int Shape::EndX() const noexcept { return x2_; }
int Shape::EndY() const noexcept { return y2_; }

#include <algorithm>
RECT Shape::Bounds() const noexcept {
    return {(std::min)(x1_, x2_), (std::min)(y1_, y2_),
        (std::max)(x1_, x2_), (std::max)(y1_, y2_)};
}
