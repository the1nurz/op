"""Створення редагованого звіту і зображення UML. Не потрібне для збирання C++."""
from pathlib import Path
import base64
import html
import math
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'Звіт'
OUT.mkdir(exist_ok=True)


def draw_uml():
    image = Image.new('RGB', (1800, 1250), 'white')
    draw = ImageDraw.Draw(image)
    font = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 23)
    bold = ImageFont.truetype('C:/Windows/Fonts/consolab.ttf', 27)
    small = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 21)

    def box(rect, title, lines):
        x1, y1, x2, y2 = rect
        draw.rectangle(rect, outline='black', width=2)
        draw.text(((x1 + x2) // 2, y1 + 22), title, font=bold, fill='black', anchor='mm')
        draw.line((x1, y1 + 46, x2, y1 + 46), fill='black', width=2)
        for index, line in enumerate(lines):
            draw.text((x1 + 14, y1 + 58 + index * 29), line, font=font, fill='black')

    def line(points, generalization=False, dashed=False, arrow=False):
        if not dashed:
            draw.line(points, fill='black', width=2)
        else:
            for a, b in zip(points, points[1:]):
                length = math.dist(a, b)
                for t in range(0, int(length), 16):
                    p = [(a[0] + (b[0] - a[0]) * u / length,
                          a[1] + (b[1] - a[1]) * u / length)
                         for u in (t, min(t + 8, length))]
                    draw.line(p, fill='black', width=2)
        if generalization or arrow:
            a, b = points[-2:]
            length = math.dist(a, b)
            ux, uy = (b[0] - a[0]) / length, (b[1] - a[1]) / length
            triangle = [b, (b[0] - ux * 22 - uy * 10, b[1] - uy * 22 + ux * 10),
                        (b[0] - ux * 22 + uy * 10, b[1] - uy * 22 - ux * 10)]
            if generalization:
                draw.polygon(triangle, fill='white', outline='black', width=2)
            else:
                draw.line((triangle[1], triangle[0], triangle[2]), fill='black', width=2)

    shape = (650, 35, 1180, 285)
    primitives = [(35, 410, 375, 560), (450, 410, 790, 560),
                  (865, 410, 1205, 560), (1280, 410, 1620, 560)]
    for index, rect in enumerate(primitives):
        mid = (rect[0] + rect[2]) // 2
        endpoint = 720 + index * 120
        bend_y = 315 + index * 18
        line([(mid, 410), (mid, bend_y), (endpoint, bend_y), (endpoint, 285)], generalization=True)
        if index:
            draw.text((mid + 8, 370), 'virtual', font=small, fill='black')

    line([(610, 745), (610, 650), (620, 650), (620, 560)], generalization=True)
    line([(830, 745), (830, 625), (980, 625), (980, 560)], generalization=True)
    line([(1210, 745), (1210, 650), (1100, 650), (1100, 560)], generalization=True)
    line([(1420, 745), (1420, 625), (1450, 625), (1450, 560)], generalization=True)
    # Композиція: редактор володіє завершеними фігурами та чернеткою.
    line([(95, 960), (15, 960), (15, 190), (650, 190)])
    draw.polygon([(95, 960), (110, 950), (125, 960), (110, 970)], fill='black')
    draw.text((35, 865), 'owns 0..128 + draft 0..1', font=small, fill='black')
    line([(1050, 1090), (780, 1090)], dashed=True, arrow=True)
    draw.text((805, 1120), 'uses', font=small, fill='black')

    box(shape, 'Shape  <<abstract>>', ['- x1_, y1_, x2_, y2_: int',
        '+ SetEnd(int, int)', '+ StartX/Y(), EndX/Y()', '+ Show(HDC, bool) = 0', '+ virtual ~Shape()'])
    for rect, title in zip(primitives, ['PointShape', 'EllipseShape', 'LineShape', 'RectShape']):
        box(rect, title, ['+ Show(HDC, bool)', '# Show(...coords...)'] if title != 'PointShape'
            else ['+ Show(HDC, bool)'])
    box((480, 745, 940, 875), 'CircleLineShape', ['+ Show(HDC, bool)'])
    box((1100, 745, 1560, 875), 'CubeShape', ['+ Show(HDC, bool)'])
    box((125, 960, 780, 1200), 'MyEditor', ['- Shape* pcshape[128]', '- Shape* draft_',
        '+ Begin / Update / Commit', '+ SetTool / Cancel / Clear', '+ Show(HDC)'])
    box((1050, 990, 1660, 1195), 'MainWindow', ['- HWND hwnd_, toolbar_, tooltip_',
        '+ Create / Run', '- OnPaint / OnCommand / OnNotify'])
    draw.text((140, 1210), 'static MyEditor editor;  // MainWindow.cpp', font=small, fill='black')
    image.save(ROOT / 'docs' / 'class-diagram.png')


draw_uml()
blocks = []


def add(kind, text, extra=None):
    blocks.append((kind, text, extra))


add('cover', 'МІНІСТЕРСТВО ОСВІТИ І НАУКИ УКРАЇНИ\n'
    'НАЦІОНАЛЬНИЙ ТЕХНІЧНИЙ УНІВЕРСИТЕТ УКРАЇНИ\n'
    '«КИЇВСЬКИЙ ПОЛІТЕХНІЧНИЙ ІНСТИТУТ ІМЕНІ ІГОРЯ СІКОРСЬКОГО»\n'
    'Факультет інформатики та обчислювальної техніки\nКафедра обчислювальної техніки\n\n'
    'ЗВІТ\nдо лабораторної роботи №4\n'
    'з дисципліни «Об’єктно-орієнтоване програмування»\n'
    'Вдосконалення структури коду графічного редактора об’єктів\n\n'
    'Виконав студент групи ІМ-53\nШкола Михайло Володимирович\nВаріант №27\n'
    'Перевірив Рекечинський Д. О.\n\nКиїв 2026')

add('h1', '1 Мета роботи')
add('p', 'Отримати навички проєктування класів та модернізувати графічний редактор '
    'в об’єктно-орієнтованому стилі, щоб нові типи об’єктів можна було додавати '
    'без переписування загального алгоритму малювання і зберігання фігур.')

add('h1', '2 Завдання для варіанта №27')
add('p', 'Номер у журналі 27 є непарним, тому редактор реалізовано як глобальний '
    'статичний об’єкт класу MyEditor. Проєкт Lab4 створено на основі Lab3. '
    'Редактор підтримує крапку, лінію, прямокутник, еліпс, лінію з кружечками '
    'на кінцях і каркас куба. Дві нові фігури реалізовано множинним успадкуванням.')
add('p', 'У пам’яті зберігається до 128 завершених фігур у масиві Shape* pcshape[kCapacity], '
    'де kCapacity = 128. Прямокутник задається протилежними кутами, еліпс '
    'задається центром і кутом охоплюючого прямокутника. Обидві фігури мають чорний '
    'контур без заповнення. Крапка зображається хрестиком, як у попередньому проєкті. '
    'Нові фігури також чорні, без заповнення. Гумовий слід усіх шести фігур пунктирний.')
add('p', 'Поточний тип фігури позначається в меню та на Toolbar. Кожна з шести кнопок '
    'має піктограму та українську підказку. Після WM_PAINT усі фігури відновлюються '
    'з даних у пам’яті. Вихід з програми звільняє створені об’єкти та GDI-ресурси.')

add('h1', '3 Аналіз Lab3 та виконані зміни')
add('p', 'У Lab3 вже були абстрактний Shape, чотири класи фігур, MyEditor, MainWindow, '
    'меню, Toolbar і допоміжні класи для безпечної роботи з GDI. Цю структуру '
    'збережено. Обробники миші створюють чернетку, змінюють її кінцеву координату '
    'та після відпускання кнопки додають об’єкт до масиву.')
add('p', 'Основні зміни: Draw перейменовано на Show; редактор винесено з поля MainWindow '
    'у глобальний статичний об’єкт; додано CircleLineShape і CubeShape; меню '
    'та Toolbar розширено з чотирьох до шести інструментів. Параметр preview '
    'тепер визначає стиль пера. Оригінальні файли Lab3 залишено без змін.')
add('p', 'У фактичному коді Lab3 еліпс мав рожеве заповнення. У Lab4 застосовано '
    'NULL_BRUSH відповідно до окремо заданої вимоги еліпса без заповнення.')

add('h1', '4 Архітектура і множинне успадкування')
add('p', 'Shape зберігає приватні координати початку та кінця, надає метод SetEnd '
    'і методи читання координат. Його Show є чисто віртуальним, а деструктор '
    'віртуальним. Через вказівник Shape* редактор викликає Show конкретного '
    'нащадка та правильно видаляє об’єкт незалежно від його типу.')
add('code', (ROOT / 'src' / 'CircleLineShape.h').read_text(encoding='utf-8') + '\n'
    + (ROOT / 'src' / 'CubeShape.h').read_text(encoding='utf-8'), 'cpp')
add('p', 'Віртуальне успадкування залишає одну спільну базу Shape у кожній складеній '
    'фігурі. Найпохідніші класи явно ініціалізують цю базу у своїх конструкторах. '
    'Таким чином, немає двох наборів координат і неоднозначного перетворення в Shape*.')
add('p', 'CircleLineShape::Show викликає LineShape::Show для основної лінії та двічі '
    'EllipseShape::Show для кружечків радіуса 6 пікселів. CubeShape::Show двічі '
    'викликає RectShape::Show для квадратних граней і чотири рази LineShape::Show '
    'для ребер між ними. Захищені перевантаження Show приймають координати '
    'окремої частини, тому вихідні координати об’єкта не змінюються під час малювання.')
add('p', 'Сторона передньої грані куба дорівнює меншому з модулів переміщення миші '
    'по X та Y. Задня грань зміщена вправо та вгору на третину сторони. '
    'Отримується квадратна передня грань і видимий каркас з двох граней та чотирьох ребер.')
add('p', 'MyEditor володіє фігурами й чернеткою. Begin створює фігуру через звичайний '
    'switch, Update змінює кінець, Commit переносить чернетку в масив, Cancel '
    'видаляє чернетку, Clear видаляє всі об’єкти. MainWindow керує вікном і '
    'передає події глобальному редактору. Динамічного створення MyEditor немає.')
add('code', 'static MyEditor editor; // на рівні файлу MainWindow.cpp', 'cpp')
add('p', 'Для гумового сліду Show отримує preview = true і створює перо PS_DOT. '
    'WM_MOUSEMOVE оновлює координати й викликає InvalidateRect, а WM_PAINT заново '
    'відображає завершені фігури та поточну чернетку. Попередній контур зникає '
    'після очищення фону. XOR-малювання не потрібне. TRANSPARENT зберігає видимими '
    'фігури під проміжками пунктиру.')

add('h1', '5 UML діаграма класів')
add('image', '../docs/class-diagram.png', 'Рисунок 1. Успадкування, володіння фігурами та використання глобального редактора')
add('p', 'Порожній трикутник вказує на базовий клас. Позначка virtual означає '
    'віртуальне успадкування Shape. Заповнений ромб позначає володіння фігурами, '
    'пунктирна стрілка — використання глобального редактора класом MainWindow. '
    'Позначення +, - та # означають public, private та protected.')

add('h1', '6 Робота програми')
add('p', 'Після вибору інструмента потрібно натиснути ліву кнопку миші на полотні, '
    'перетягнути мишу та відпустити кнопку. Для крапки достатньо клацання. '
    'Вибір нового інструмента скасовує незавершену чернетку. Завершені фігури '
    'залишаються в пам’яті до закриття програми.')
for filename, caption in [
    ('all-shapes.png', 'Рисунок 2. Шість фігур, створених мишею у запущеному Lab4'),
    ('menu-ellipse.png', 'Рисунок 3. Вибір еліпса синхронізовано між меню та Toolbar'),
    ('after-repaint.png', 'Рисунок 4. Фігури після розгортання та відновлення вікна')]:
    add('image', '../docs/screenshots/' + filename, caption)

add('h1', '7 Результати перевірки')
add('p', '17 листопада 2026 року проєкт скомпільовано у Windows компілятором GNU C++ '
    '16.2.0 з MinGW UCRT64 через CMake у конфігурації Debug. Створено виконуваний '
    'файл Lab4.exe. У фінальному складанні помилок і попереджень не було. '
    'CTest виконав три тести, усі три завершилися успішно.')
add('code', 'Lab4ModelSmoke    Passed\nLab4EditorSmoke   Passed\n'
    'Lab4WindowSmoke   Passed\n100% tests passed, 0 tests failed out of 3', 'text')
add('p', 'Lab4ModelSmoke перевіряє малювання шести фігур, координати, відсутність '
    'заповнення, відмінність пунктиру від суцільного контуру та однаковість '
    'результату повторного малювання. Також перевіряється відновлення вибраних '
    'пера, пензля і режиму фону. Після 500 циклів малювання всіх фігур '
    'кількість GDI-об’єктів процесу не збільшилася.')
add('p', 'Lab4EditorSmoke перевіряє всі типи інструментів, завершення та скасування '
    'введення, скасування при зміні інструмента, некоректні команди, межу '
    '128 об’єктів і очищення. Lab4WindowSmoke створює окреме приховане вікно '
    'та перевіряє шість кнопок, шість зображень, шість пунктів меню, синхронізацію '
    'позначок і українські тексти всіх шести tooltip-повідомлень.')
add('p', 'У запущеному редакторі мишею створено кожний тип фігури, перевірено '
    'перемикання інструментів і збереження зображення після зміни розміру вікна. '
    'Для звіту зроблено справжні скріншоти. Безпосереднє складання MSVC '
    'не виконувалося через відсутність Visual Studio у середовищі. Час появи '
    'popup-підказок не вимірювався; автоматично перевірено їхній обробник і тексти.')

add('h1', '8 Контрольні запитання')
answers = [
    ('Що таке поліморфізм і як він використаний у цій лабораторній роботі?',
     'Поліморфізм дозволяє працювати з об’єктами різних похідних класів через '
     'спільний інтерфейс базового класу. У масиві редактора зберігаються Shape*. '
     'Виклик pcshape[i]->Show(dc) під час виконання обирає перевизначений Show '
     'відповідної фігури. Редактору не потрібно визначати конкретний тип об’єкта '
     'в циклі малювання. Віртуальний деструктор забезпечує коректне видалення '
     'через вказівник на базовий клас.'),
    ('Обробку яких повідомлень потрібно виконувати для вводу об’єктів?',
     'WM_LBUTTONDOWN починає створення фігури та захоплює мишу через SetCapture; '
     'WM_MOUSEMOVE оновлює координати чернетки; WM_LBUTTONUP завершує фігуру '
     'і звільняє захоплення. WM_PAINT відображає об’єкти й гумовий слід. '
     'WM_COMMAND обирає інструмент із меню або Toolbar. WM_CAPTURECHANGED '
     'і WM_CANCELMODE скасовують незавершений ввід при втраті захоплення. '
     'WM_NOTIFY обслуговує підказки, а WM_INITMENUPOPUP оновлює позначку меню.'),
    ('Що таке абстрактний клас і скільки їх у цій програмі?',
     'Абстрактний клас має принаймні одну чисто віртуальну функцію, для якої '
     'немає кінцевого перевизначення у цьому класі. Створити його об’єкт '
     'безпосередньо не можна. У програмі один абстрактний клас — Shape '
     'з методом virtual void Show(HDC dc, bool preview = false) const = 0. '
     'Усі шість класів фігур мають власне перевизначення Show, тому є конкретними.'),
    ('Що таке множинне успадкування і як воно впливає на модульність?',
     'Множинне успадкування означає, що клас має більше одного безпосереднього '
     'базового класу. CircleLineShape успадковує LineShape і EllipseShape, '
     'а CubeShape — LineShape і RectShape. Це дозволяє повторно використати '
     'реалізацію окремих геометричних примітивів у новому модулі. Зміни '
     'малювання примітива зосереджено у його класі. Водночас нащадок залежить '
     'від кількох базових класів, тому потрібно уникати неоднозначності '
     'імен і зайвої залежності від внутрішніх даних батьків.'),
    ('Що таке ромбічне спадкування та які проблеми воно може спричинити?',
     'Ромб виникає, коли клас успадковує два класи, що вже мають спільну базу. '
     'Без віртуального успадкування складена фігура отримала б дві частини '
     'Shape: два набори координат, неоднозначний доступ до методів '
     'та неоднозначне перетворення в Shape*. У цій програмі проміжні класи '
     'використовують virtual public Shape, тому спільна база лише одна. '
     'Її конструктор викликає найпохідніший клас. Однакові назви Show '
     'у батьків уточнюються як LineShape::Show, RectShape::Show '
     'та EllipseShape::Show. Найпохідніший клас також перевизначає '
     'віртуальний Show, щоб мати єдиний кінцевий метод.'),
    ('Як додати нові кнопки у Toolbar?',
     'Потрібно визначити новий ID команди й ресурс піктограми, додати bitmap '
     'до ресурсного файлу та завантажити його в ImageList. Далі додати '
     'структуру TBBUTTON з індексом зображення, ID команди, станом '
     'TBSTATE_ENABLED і стилем BTNS_CHECKGROUP. Після цього передати '
     'масив кнопок через TB_ADDBUTTONS. Для нових ID потрібно розширити '
     'обробку WM_COMMAND, синхронізацію через TB_CHECKBUTTON '
     'і CheckMenuRadioItem та текст підказки в TTN_GETDISPINFOW. '
     'У Lab4 усі ці зміни виконано для команд 14 і 15.')]
for index, (question, answer) in enumerate(answers, 1):
    add('h2', f'{index} {question}')
    add('p', answer)

add('h1', '9 Висновки')
add('p', 'Під час роботи я вдосконалив структуру редактора з Lab3 та додав два типи '
    'фігур через множинне успадкування. Повторне використання Show базових '
    'класів дозволило не дублювати малювання ліній, прямокутників та еліпсів. '
    'Віртуальна база Shape усунула дублювання координат у ромбі успадкування. '
    'Глобальний статичний MyEditor відповідає варіанту №27. Програма '
    'підтримує шість інструментів, пунктирний гумовий слід, синхронізацію '
    'меню і Toolbar та повторне малювання збережених об’єктів. '
    'Складання MinGW, автоматичні тести й перевірка основних дій у GUI пройшли успішно.')

add('h1', '10 Вихідні тексти програми')
add('p', 'Нижче наведено повні тексти головного файлу, класів, допоміжного модуля GDI '
    'та конфігурації збирання. Файли .h містять оголошення, .cpp — реалізації. '
    'Бітові зображення кнопок зберігаються в папці resources проєкту.')
paths = [ROOT / 'src' / 'main.cpp']
for name in ['Shape', 'PointShape', 'LineShape', 'RectShape', 'EllipseShape',
             'CircleLineShape', 'CubeShape', 'MyEditor', 'MainWindow']:
    paths += [ROOT / 'src' / (name + '.h'), ROOT / 'src' / (name + '.cpp')]
paths += [ROOT / 'src' / 'GdiGuards.h', ROOT / 'resources' / 'resource.h',
          ROOT / 'resources' / 'Lab4.rc.in', ROOT / 'CMakeLists.txt']
for path in paths:
    add('h2', path.relative_to(ROOT).as_posix())
    add('code', path.read_text(encoding='utf-8'), 'cmake' if path.suffix == '.txt' else 'cpp')

add('h1', '11 Вихідні тексти перевірок')
for path in sorted((ROOT / 'tests').glob('*.cpp')):
    add('h2', path.relative_to(ROOT).as_posix())
    add('code', path.read_text(encoding='utf-8'), 'cpp')
add('h1', '12 Використані матеріали')
add('p', 'Методичні вказівки oop-practicum-simplified (3).pdf, сторінки 14–16. '
    'Вихідний проєкт Lab3 та його звіт. Вимоги користувача для варіанта №27.')

md = []
web = ['<!doctype html><html lang="uk"><head><meta charset="utf-8">',
       '<title>Звіт Lab4 Варіант 27</title><style>',
       '@page {size:A4; margin:20mm 20mm 20mm 25mm} '
       'body{font:14pt/1.45 "Times New Roman",serif;color:#000;max-width:170mm;margin:20mm auto;background:white} '
       'h1{font-size:18pt;line-height:1.25;margin:20pt 0 10pt;break-after:avoid} '
       'h2{font-size:14pt;line-height:1.3;margin:15pt 0 8pt;break-after:avoid} '
       'p{margin:0 0 10pt;text-align:justify} '
       'pre{font:8.5pt/1.25 Consolas,"Courier New",monospace;white-space:pre-wrap;overflow-wrap:anywhere;tab-size:4;margin:8pt 0 16pt} '
       '.cover{text-align:center;min-height:245mm;break-after:page} '
       '.cover p{text-align:center;margin:0 0 10pt} '
       'figure{margin:15pt 0;break-inside:avoid} '
       'img{max-width:100%;height:auto;display:block;margin:auto} '
       'figcaption{text-align:center;font-size:12pt;margin-top:8pt} '
       '@media print{body{margin:0;max-width:none}}',
       '</style></head><body>']
for kind, text, extra in blocks:
    escaped = html.escape(text)
    if kind == 'cover':
        md.append(text.replace('\n', '  \n') + '\n\n---\n')
        web.append('<section class="cover">' + ''.join(
            '<p>' + html.escape(line) + '</p>' if line else '<br>' for line in text.splitlines()) + '</section>')
    elif kind in ('h1', 'h2'):
        md.append(('#' if kind == 'h1' else '##') + ' ' + text + '\n')
        web.append(f'<{kind}>{escaped}</{kind}>')
    elif kind == 'p':
        md.append(text + '\n')
        web.append('<p>' + escaped + '</p>')
    elif kind == 'code':
        md.append('```' + extra + '\n' + text.rstrip() + '\n```\n')
        web.append('<pre><code>' + escaped.rstrip() + '</code></pre>')
    elif kind == 'image':
        md.append('![' + extra + '](' + text + ')\n\n' + extra + '\n')
        data = base64.b64encode((OUT / text).read_bytes()).decode('ascii')
        web.append(f'<figure><img alt="{html.escape(extra)}" src="data:image/png;base64,{data}">'
                   f'<figcaption>{html.escape(extra)}</figcaption></figure>')
web.append('</body></html>')
stem = OUT / 'Звіт_Lab4_Варіант_27'
stem.with_suffix('.md').write_text('\n'.join(md), encoding='utf-8')
stem.with_suffix('.html').write_text('\n'.join(web), encoding='utf-8')
print(f'Created {len(paths)} complete application/config listings and 3 test listings.')
print(stem.with_suffix('.md'))
print(stem.with_suffix('.html'))
