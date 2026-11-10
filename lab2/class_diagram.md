# Успадкування класів
```mermaid
classDiagram
    Shape <|-- PointShape
    Shape <|-- LineShape
    Shape <|-- RectShape
    Shape <|-- EllipseShape
    class Shape {
        <<abstract>>
        #int x1, y1, x2, y2
        +SetEnd(int x, int y)
        +Draw(HDC dc, bool preview)*
        +~Shape() virtual
    }
```
Чотири класи перевизначають Draw. main.cpp володіє динамічним масивом Shape** pcshape на 127 елементів. Окремий вказівник draft містить поточну незавершену фігуру.
Рисунок для звіту: [class_diagram.png](Звіт/class_diagram.png).
