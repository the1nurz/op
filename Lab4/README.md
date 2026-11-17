# Лабораторна робота №4

Вдосконалення структури коду графічного редактора об'єктів. Варіант №27.
Проєкт створено на основі `../lab3`. Оригінальні файли Lab3 не змінено.
Використано C++17, WinAPI, GDI та стандартний Toolbar із `comctl32`.

## Що змінилося відносно Lab3

- `MyEditor` перенесено з поля `MainWindow` у глобальний статичний об'єкт
  `static MyEditor editor;` у `src/MainWindow.cpp`. Динамічного `MyEditor` немає.
- `Draw` перейменовано на `Show`, як у методичці. Під час прев'ю використовується
  `PS_DOT`, під час остаточного малювання — `PS_SOLID`. Товщина пера — 1 піксель.
- Додано `CircleLineShape : public LineShape, public EllipseShape` і
  `CubeShape : public LineShape, public RectShape`.
- `LineShape`, `EllipseShape`, `RectShape` віртуально успадковують `Shape`.
  Складена фігура містить одну спільну базу `Shape` з координатами.
- Захищені перевантаження `Show` батьківських класів приймають координати
  частини фігури. Вони дозволяють малювати кілька ребер чи кружечків без зміни
  координат усього об'єкта та без дублювання GDI-коду.
- У меню й Toolbar тепер шість інструментів. Додано дві піктограми й підказки.
- Еліпс зроблено без заповнення за прямою вимогою користувача. У фактичному
  Lab3 еліпс був рожевим; цю розбіжність усунуто тільки в Lab4.

Збережено масив `Shape* pcshape[kCapacity]`, де `kCapacity = 128`, обробники
подій миші, перемальовування через `WM_PAINT` та допоміжні класи GDI з Lab3.

## Структура файлів

```text
Lab4/
  .gitignore
  CMakeLists.txt
  README.md
  src/
    main.cpp
    MainWindow.h / MainWindow.cpp
    MyEditor.h / MyEditor.cpp
    Shape.h / Shape.cpp
    PointShape.h / PointShape.cpp
    LineShape.h / LineShape.cpp
    RectShape.h / RectShape.cpp
    EllipseShape.h / EllipseShape.cpp
    CircleLineShape.h / CircleLineShape.cpp
    CubeShape.h / CubeShape.cpp
    GdiGuards.h
  resources/
    resource.h
    Lab4.rc.in
    dot.bmp / line.bmp / rectangle.bmp / ellipse.bmp
    circle-line.bmp / cube.bmp
  tests/
    model_smoke.cpp
    editor_smoke.cpp
    window_smoke.cpp
  docs/
    class-diagram.mmd
    class-diagram.png
    verification.md
    screenshots/
  tools/
    make_report.py
  Звіт/
    Звіт_Lab4_Варіант_27.md
    Звіт_Lab4_Варіант_27.html
```

Нові класи фігур розміщено в `src`, піктограми — у `resources`, схему та
результати перевірок — у `docs`. Повні тексти всіх файлів програми й конфігурації
наведено у звіті. Файли вже розкладено по папках; вставляти фрагменти вручну не потрібно.
`tmp`, `build`, `out` та `.vs` — службові папки, виключені з Git.

## Запуск у Visual Studio

1. У Visual Studio Installer має бути встановлено компонент **Desktop development
   with C++**, Windows SDK та підтримку CMake для C++.
2. Вибрати **File → Open → Folder** і відкрити саме папку `Lab4`, у якій є
   `CMakeLists.txt`.
3. Дочекатися конфігурації CMake. Вибрати конфігурацію `x64-Debug`.
4. Вибрати `Lab4.exe` як Startup Item та натиснути `Ctrl+F5` або `F5`.

Окремий `.sln` не обов'язковий. За потреби його можна створити в
Developer PowerShell for Visual Studio з кореня репозиторію:

```powershell
cmake -S Lab4 -B Lab4/out/vs -G "Visual Studio 17 2022" -A x64 -DLAB4_BUILD_TESTS=ON
cmake --build Lab4/out/vs --config Debug
ctest --test-dir Lab4/out/vs -C Debug --output-on-failure
.\Lab4\out\vs\Debug\Lab4.exe
```

Назва генератора наведена для Visual Studio 2022. Для іншої версії відкрити
папку через IDE або обрати відповідний генератор зі списку `cmake --help`.
MSVC у поточному середовищі відсутній, тому тут фактично перевірено MinGW,
а складання безпосередньо у Visual Studio потрібно перевірити на машині з MSVC.

## Збирання встановленим MinGW

У цьому середовищі виконано такі команди з кореня репозиторію:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
cmake -S Lab4 -B Lab4/build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DLAB4_BUILD_TESTS=ON -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_RC_COMPILER=C:/msys64/ucrt64/bin/windres.exe -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/mingw32-make.exe
cmake --build Lab4/build --parallel 4
ctest --test-dir Lab4/build --output-on-failure
.\Lab4\build\Lab4.exe
```

Готовий локальний виконуваний файл: `build/Lab4.exe`.
Папка збирання не включається в Git; після завантаження джерел її створює CMake.

## Як користуватися

Вибрати інструмент у меню «Об'єкти» або на Toolbar. Натиснути ліву кнопку миші
на полотні, перетягнути та відпустити. Крапка, як у Lab3, зображається маленьким
хрестиком у місці початкового натискання.

Прямокутник задається протилежними кутами. Еліпс задається центром і кутом
охоплюючого прямокутника. Для лінії з кружечками задаються кінці лінії;
радіус кружечків — 6 пікселів. Для куба задаються початкова вершина та протилежний
кут передньої грані; її сторона дорівнює меншому зі зміщень миші по X та Y,
тому грань квадратна. Задня грань зміщена вправо і вгору на третину сторони.
Можна вводити фігури в будь-якому напрямку.

Поки кнопка миші натиснута, видно пунктирний контур. Після відпускання фігура
зберігається в масиві та малюється суцільною лінією. При втраті захоплення миші
незавершена фігура скасовується. Після 128 завершених фігур програма повідомляє
про досягнення межі. Після закриття програми фігури звільняються з пам'яті.

## UML, звіт і перевірки

- [UML як зображення](docs/class-diagram.png).
- [Редаговане джерело UML Mermaid](docs/class-diagram.mmd).
- [Перевірка вимог та фактичні результати](docs/verification.md).
- [Звіт Markdown](Звіт/Звіт_Lab4_Варіант_27.md).
- [Звіт HTML](Звіт/Звіт_Lab4_Варіант_27.html).

Обидва формати звіту редагуються. HTML можна відкрити у браузері для друку або
в Microsoft Word для редагування та збереження як DOCX. Зображення в HTML вбудовані,
тому для його перенесення достатньо одного файлу. У звіті є всі шість відповідей
зі сторінки 16 методички, повні джерела, UML і справжні скріншоти.

Для додавання наступної фігури потрібно створити нащадка `Shape` з `Show`, додати
його створення у `MyEditor::CreateShape`, пункт меню/назву/кнопку, піктограму
й файл `.cpp` у CMake. Цикл малювання та очищення масиву змінювати не потрібно.
