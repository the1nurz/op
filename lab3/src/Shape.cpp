#include "Shape.h"

Shape::Shape(int x, int y) noexcept : x1_(x), y1_(y), x2_(x), y2_(y) {}
Shape::~Shape() = default;

void Shape::SetEnd(int x, int y) noexcept { x2_ = x; y2_ = y; }
int Shape::StartX() const noexcept { return x1_; }
int Shape::StartY() const noexcept { return y1_; }
int Shape::EndX() const noexcept { return x2_; }
int Shape::EndY() const noexcept { return y2_; }
