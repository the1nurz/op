"""Звіт Markdown/HTML і UML PNG. Залежність: Pillow; --docx створює чернетку для QA."""
from pathlib import Path
import argparse
import base64
import html
import math
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'Звіт'
OUT.mkdir(exist_ok=True)


def draw_uml():
    image = Image.new('RGB', (1800, 1660), 'white')
    draw = ImageDraw.Draw(image)
    font = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 22)
    bold = ImageFont.truetype('C:/Windows/Fonts/consolab.ttf', 26)
    small = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 20)

    def box(rect, title, lines):
        x1, y1, x2, y2 = rect
        draw.rectangle(rect, outline='black', width=2)
        draw.text(((x1+x2)//2, y1+22), title, font=bold, fill='black', anchor='mm')
        draw.line((x1,y1+46,x2,y1+46),fill='black',width=2)
        for i, text in enumerate(lines):
            draw.text((x1+14,y1+58+i*29),text,font=font,fill='black')

    def line(points, triangle=False, dashed=False, arrow=False):
        for a,b in zip(points,points[1:]):
            length=math.dist(a,b)
            if not dashed:
                draw.line([a,b],fill='black',width=2)
            elif length:
                for t in range(0,int(length),16):
                    pair=[(a[0]+(b[0]-a[0])*u/length,a[1]+(b[1]-a[1])*u/length)
                          for u in (t,min(t+8,length))]
                    draw.line(pair,fill='black',width=2)
        if triangle or arrow:
            a,b=points[-2:]
            length=math.dist(a,b)
            ux,uy=(b[0]-a[0])/length,(b[1]-a[1])/length
            tri=[b,(b[0]-ux*22-uy*10,b[1]-uy*22+ux*10),
                 (b[0]-ux*22+uy*10,b[1]-uy*22-ux*10)]
            if triangle: draw.polygon(tri,fill='white',outline='black',width=2)
            else: draw.line([tri[1],tri[0],tri[2]],fill='black',width=2)

    shape=(620,30,1220,290)
    primitives=[(30,410,370,550),(460,410,800,550),(890,410,1230,550),(1320,410,1660,550)]
    for i,rect in enumerate(primitives):
        mid=(rect[0]+rect[2])//2
        end=680+i*150
        bend=320+i*18
        line([(mid,410),(mid,bend),(end,bend),(end,290)],triangle=True)
        if i: draw.text((mid+8,375),'virtual',font=small,fill='black')
    line([(560,700),(560,620),(630,620),(630,550)],triangle=True)
    line([(820,700),(820,590),(1040,590),(1040,550)],triangle=True)
    line([(1190,700),(1190,640),(1110,640),(1110,550)],triangle=True)
    line([(1430,700),(1430,610),(1490,610),(1490,550)],triangle=True)
    # Володіння редактором фігурами.
    line([(100,900),(12,900),(12,155),(620,155)])
    draw.polygon([(100,900),(115,890),(130,900),(115,910)],fill='black')
    draw.text((25,825),'owns',font=small,fill='black')
    line([(1070,960),(820,960)],dashed=True,arrow=True)
    draw.text((850,925),'Instance()',font=small,fill='black')
    # MainWindow володіє таблицею. Зворотний виклик не є залежністю MyTable від MainWindow.
    line([(1390,1170),(1390,1320)])
    draw.polygon([(1390,1170),(1380,1185),(1390,1200),(1400,1185)],fill='black')
    draw.text((1410,1230),'owns',font=small,fill='black')
    line([(1060,1400),(990,1400),(990,1050),(1070,1050)],dashed=True,arrow=True)
    draw.text((1000,1205),'callback',font=small,fill='black')
    line([(455,1195),(455,1320)],dashed=True,arrow=True)
    draw.text((470,1250),'create',font=small,fill='black')
    box(shape,'Shape <<abstract>>',['- x1_, y1_, x2_, y2_: int','+ SetEnd / coordinates',
        '+ Show(HDC, bool) = 0','+ TypeId() = 0 / Bounds()','+ virtual ~Shape()'])
    for rect,title in zip(primitives,['PointShape','EllipseShape','LineShape','RectShape']):
        box(rect,title,['+ Show / TypeId / Bounds'] if title in ['PointShape','EllipseShape'] else ['+ Show / TypeId'])
    box((450,700,930,820),'CircleLineShape',['+ Show / TypeId / Bounds'])
    box((1070,700,1550,820),'CubeShape',['+ Show / TypeId / Bounds'])
    box((130,855,820,1195),'MyEditor <<Meyers Singleton>>',['- MyEditor() / ~MyEditor()',
        '- Shape* pcshape[128], draft_','- selected_: int','+ static Instance(): MyEditor&',
        '+ Begin / Update / Commit / Clear','+ Select / Delete / At / Show','+ Save / Load'])
    box((1070,900,1740,1170),'MainWindow',['- MyTable table_', '+ Create / Run',
        '- RefreshTable / FileCommand','- static TableEvent(context,event,row)','- OnPaint / mouse / menu'])
    box((130,1320,840,1515),'ShapeFactory <<module>>',['+ ShapeTypes / FindShapeType',
        '+ CreateShapeByType(id,x,y): Shape*','catalog: id, name, bitmap, constructor'])
    box((1060,1320,1740,1575),'MyTable',['- HWND hwnd_, list_', '- Callback callback_; void* context_',
        '+ Open / Close / ProcessMessage', '+ SetRows(TableRow values, selected)',
        '+ SetCallback(callback, context)'])
    draw.text((80,1585),'Triangle = inheritance; diamond = ownership; dashed = use / callback',font=small,fill='black')
    draw.text((80,1620),'MyTable has no dependency on editor or shapes. Full UML: class-diagram.mmd',font=small,fill='black')
    image.save(ROOT/'docs/class-diagram.png')


draw_uml()
blocks=[]
def add(kind,text,extra=''): blocks.append((kind,text,extra))

add('cover','МІНІСТЕРСТВО ОСВІТИ І НАУКИ УКРАЇНИ\n'
    'НАЦІОНАЛЬНИЙ ТЕХНІЧНИЙ УНІВЕРСИТЕТ УКРАЇНИ\n'
    '«КИЇВСЬКИЙ ПОЛІТЕХНІЧНИЙ ІНСТИТУТ ІМЕНІ ІГОРЯ СІКОРСЬКОГО»\n'
    'Факультет інформатики та обчислювальної техніки\nКафедра обчислювальної техніки\n\n'
    'ЗВІТ\nдо лабораторної роботи №5\n'
    'з дисципліни «Об’єктно-орієнтоване програмування»\n'
    'Розробка багатовіконного інтерфейсу користувача для графічного редактора об’єктів\n\n'
    'Виконав студент групи ІМ-53\nШкола Михайло Володимирович\nВаріант №27\n'
    'Перевірив Рекечинський Д. О.\n\nКиїв 2026')

add('h1','1 Мета роботи')
add('p','Отримати вміння та навички програмувати багатовіконний інтерфейс в об’єктно-орієнтованому стилі. '
    'У роботі редактор Lab4 доповнено немодальною таблицею, збереженням і завантаженням об’єктів, '
    'а єдиний MyEditor реалізовано як Singleton Меєрса. Реалізовано всі три бонусні можливості.')
add('h1','2 Варіант завдання 27')
add('p','За методичкою «oop-practicum-simplified (3).pdf», сторінки 17–19, '
    'для непарних номерів потрібен Singleton Меєрса. Номер 27 є непарним. '
    'Необхідно зберегти кольори та стилі Lab4, додати клас немодальної таблиці в окремому модулі, '
    'окремий пункт меню для її відкриття і закриття та запис множини об’єктів у файл. '
    'Бонуси: виділення через рядок таблиці, видалення з масиву та завантаження файлу. '
    'Для виділення і видалення заборонені нові залежності модуля таблиці від модулів редактора.')
add('h1','3 Аналіз основи та реалізація')
add('p','Проєкт створено копіюванням потрібних джерел і ресурсів Lab4 у Lab5. '
    'Збережено Shape, PointShape, LineShape, RectShape, EllipseShape, CircleLineShape, CubeShape, '
    'MainWindow, MyEditor та класи захисту GDI. Оригінальну папку Lab4 не змінено. '
    'Успадковано масив Shape* pcshape[128], чернетку, обробники миші й WM_PAINT. '
    'Лінія з кружечками успадковує LineShape та EllipseShape, куб — LineShape та RectShape; '
    'проміжні класи віртуально успадковують Shape, тож складена фігура має одну спільну базу.')
add('h2','3.1 Singleton Меєрса')
add('code','MyEditor& MyEditor::Instance() noexcept {\n    static MyEditor instance;\n    return instance;\n}')
add('p','Конструктор і деструктор MyEditor приватні. Копіювання та присвоювання видалені; '
    'неявного переміщення також немає. Перший виклик Instance створює локальний static, '
    'ініціалізація якого потокобезпечна з C++11. Він автоматично знищується при нормальному завершенні '
    'програми. Самі методи редактора викликаються з одного GUI-потоку. Глобального static MyEditor '
    'з Lab4 більше немає. Деструктор викликає Clear, який видаляє чернетку і всі фігури.')
add('h2','3.2 Немодальна таблиця та callback')
add('p','MyTable оголошено у MyTable.h та реалізовано у MyTable.cpp. Ресурс IDD_TABLE містить '
    'ListView і дві кнопки. Open викликає CreateDialogParamW і ShowWindow. Головне вікно не вимикається. '
    'Цикл повідомлень викликає IsDialogMessageW через ProcessMessage, тому працює клавіатурна навігація. '
    'Close викликає DestroyWindow, а не EndDialog. Повторний Open активує існуючий діалог. '
    'Пункт меню перемикає його стан; WM_CLOSE і вихід із головної програми закривають таблицю.')
add('code','using Callback = void (*)(void* context, Event event, int row);\n'
    'table_.SetCallback(&MainWindow::TableEvent, this);')
add('p','Таблиця отримує вектор простих TableRow: назву та чотири координати. Вона не включає '
    'MyEditor.h, Shape.h або MainWindow.h. Події Select, Delete та Closed передаються через '
    'вказівник на функцію і непрозорий контекст void*. MainWindow викликає Select або Delete '
    'редактора й оновлює зображення. Під час перебудови рядків прапорець updating_ пригнічує '
    'службові повідомлення вибору та запобігає рекурсивним callback.')
add('h2','3.3 Виділення та видалення')
add('p','Вибраний індекс зберігається в MyEditor. Після звичайного малювання Show додає синю '
    'пунктирну рамку навколо Bounds вибраної фігури. Її чорний контур і заповнення не змінюються. '
    'Похідні класи уточнюють Bounds для крапки, еліпса, кружечків і куба. Delete перевіряє '
    'індекс, викликає delete через Shape*, зсуває наступні вказівники, очищує останню комірку '
    'та виправляє вибір. Віртуальний деструктор Shape забезпечує коректне видалення нащадка. '
    'Після цього MainWindow перебудовує таблицю й інвалідує полотно.')
add('h2','3.4 Каталог типів та поліморфізм')
add('p','ShapeFactory містить каталог ID, назв, ресурсів bitmap і функцій створення. '
    'Редактор створює фігури через CreateShapeByType. Меню, Toolbar і підказки використовують '
    'той самий каталог. Shape надає віртуальні Show, TypeId і Bounds. Таблиця та цикли '
    'малювання і серіалізації не залежать від конкретного нащадка. Для нового типу додаються '
    'клас, запис каталогу, ресурс піктограми та джерело в CMake; алгоритми наявних модулів '
    'редактора і таблиці переписувати не потрібно.')
add('h1','4 Формат файлу та перевірка даних')
add('p','Текстовий формат має версію та кількість об’єктів. Кожний наступний запис містить '
    'ID типу і початкові та кінцеві координати. ID 10–15 позначають відповідно крапку, лінію, '
    'прямокутник, еліпс, лінію з кружечками та куб. Для еліпса перша точка — центр, '
    'друга задає радіуси, як у Lab4. Для куба це точки введення, з яких обчислюються грані. '
    'Тип визначає стиль, тому колір окремо не записується. Приклад містить усі шість типів.')
add('code',(ROOT/'docs/example.txt').read_text(encoding='utf-8'))
add('p','Save готує весь текст, створює тимчасовий файл у цільовій папці, перевіряє запис, '
    'FlushFileBuffers і CloseHandle, після чого замінює ціль через MoveFileExW. '
    'При невдалій заміні попередній файл зберігається. Діалог збереження підтверджує перезапис. '
    'Load допускає лише версію 1, до 128 об’єктів, файл до 64 КіБ і координати '
    'від −1000000 до 1000000. Невідомі типи, неповні записи, переповнення чисел і зайві '
    'дані відхиляються. Об’єкти спочатку створюються в тимчасовому векторі unique_ptr; '
    'поточний масив замінюється лише після повного успіху. На помилці тимчасові об’єкти '
    'автоматично видаляються. Поточні фігури й вибір залишаються без змін.')
add('h1','5 UML діаграма класів')
add('image','../docs/class-diagram.png','Рисунок 1. Успадкування, володіння та взаємодія модулів')
add('p','Трикутник вказує на базовий клас, ромб позначає володіння, пунктир — використання '
    'або передавання callback. Стрілка callback не означає включення MainWindow.h у MyTable. '
    'Повна редагована схема Mermaid міститься в docs/class-diagram.mmd.')
add('h1','6 Робота програми')
add('p','Обрати інструмент у меню або на Toolbar, натиснути ліву кнопку на полотні, '
    'перетягнути й відпустити. Для крапки достатньо клацання. Пункт «Таблиця об’єктів» '
    'відкриває і закриває немодальний діалог. Вибір рядка виділяє відповідну фігуру; '
    'кнопка видалення або Delete прибирає її. Меню «Файл» містить відкриття і збереження. '
    'Перед завантаженням поверх непорожнього малюнка потрібно підтвердити заміну.')
for filename, caption in [
    ('all-shapes.png','Рисунок 2. Справжній Lab5 після відкриття example.txt з усіма шістьма типами'),
    ('table-selection.png','Рисунок 3. П’ятий рядок таблиці виділяє лінію з кружечками на полотні'),
    ('after-delete.png','Рисунок 4. Після видалення лінії лишилися п’ять рядків і п’ять фігур')]:
    add('image','../docs/screenshots/'+filename,caption)
add('h1','7 Результати перевірки')
add('p','18 листопада 2026 року виконано збирання Windows-програми через CMake '
    'з GNU C++ 16.2.0 MinGW UCRT64, конфігурація Debug. У фінальному складанні '
    'помилок і попереджень не виявлено. Усі три тести CTest пройшли.')
add('code','Lab5ModelSmoke    Passed\nLab5EditorSmoke   Passed\nLab5WindowSmoke   Passed\n'
    '100% tests passed, 0 tests failed out of 3')
add('p','ModelSmoke перевіряє шість фігур, пікселі повторного малювання, пунктир, '
    'відсутність заповнення і відновлення стану GDI. Після 500 циклів малювання '
    'кількість GDI-об’єктів не збільшується. EditorSmoke перевіряє властивості Singleton, '
    'життєвий цикл чернетки, межу масиву, видалення й файловий round trip шести типів, '
    'порожнього набору і 128 фігур, Unicode-шляхи, пошкоджені файли та помилки запису. '
    'Зокрема перевірено збереження початкового файлу при заблокованій заміні.')
add('p','WindowSmoke створює справжнє головне вікно і діалог. Перевірено малювання '
    'при відкритій таблиці, оновлення рядків, callback вибору й видалення, '
    'повторне відкриття та автоматичне закриття таблиці. 200 перемальовувань '
    'із виділенням не збільшили кількість GDI-об’єктів. У видимому GUI перевірено '
    'відкриття прикладу, вибір п’ятого рядка, видалення відповідної фігури й вихід. '
    'Наведені скріншоти зроблено із запущеної програми. Повний протокол — docs/verification.md.')
add('p','MSVC не перевірявся через відсутність Visual Studio. Окремий аналізатор купи '
    'не використовувався; звільнення пам’яті перевірено за володінням у коді '
    'та сценаріями очищення, видалення й невдалого завантаження. Тексти tooltip '
    'перевірено автоматично, час появи їх popup не вимірювався.')
add('h1','8 Відповіді на контрольні запитання')
answers=[
    ('Що таке патерн проєктування',
     'Патерн проєктування — узагальнений спосіб розв’язання повторюваної задачі '
     'проєктування програм. Він описує ролі об’єктів, їхні зв’язки та умови застосування, '
     'а не готовий фрагмент програми для будь-якого випадку. У цій роботі Singleton '
     'керує єдиним екземпляром редактора, а callback послаблює залежності між вікнами.'),
    ('Що таке Singleton',
     'Singleton — патерн, який забезпечує єдиний екземпляр класу та точку доступу до нього. '
     'У MyEditor створення обмежене приватним конструктором, копіювання заборонене, '
     'а Instance повертає посилання на єдиний об’єкт. Singleton доречний тут за вимогою '
     'варіанта; він також створює спільний стан, тому в інших задачах його слід застосовувати обдумано.'),
    ('Чим відрізняється класична реалізація Singleton від Singleton Меєрса',
     'Класична реалізація часто зберігає статичний вказівник, виділяє об’єкт через new '
     'і потребує явного рішення щодо видалення та синхронізації створення. '
     'Singleton Меєрса використовує локальний static усередині функції Instance. '
     'Він створюється при першому зверненні й автоматично знищується при нормальному '
     'завершенні. З C++11 його ініціалізація потокобезпечна, але це не робить '
     'автоматично потокобезпечними інші методи класу. Для непарного варіанта 27 використано Меєрса.'),
    ('Як запрограмувати немодальне діалогове вікно',
     'Потрібно описати ресурс DIALOGEX, написати DLGPROC і створити діалог через '
     'CreateDialogParamW. У WM_INITDIALOG зберігається this у DWLP_USER та налаштовуються '
     'контроли. Діалог показується ShowWindow, а його повідомлення обробляються загальним '
     'циклом із IsDialogMessageW. Головне вікно залишається активним. Закриття виконує '
     'DestroyWindow; EndDialog призначений для модальних діалогів DialogBox.'),
    ('Як запрограмувати запис у файл об’єктів геометричних форм',
     'Для кожного об’єкта треба записати стійкий ідентифікатор типу та дані, достатні '
     'для відновлення, тут чотири координати. Вказівники й сирий образ пам’яті C++-об’єкта '
     'записувати не можна: вони містять адреси та службові дані, які не відновлюють об’єкт. '
     'У Save використано TypeId і методи координат, перевірено всі операції запису. '
     'Load перевіряє формат, створює потрібного нащадка через фабрику й установлює координати.'),
    ('Поліморфізм у програмі',
     'Масив Shape* містить об’єкти різних похідних класів. Вираз pcshape[i]->Show(dc) '
     'викликає Show фактичної фігури через віртуальну диспетчеризацію. '
     'Так само TypeId повертає ID конкретного нащадка, а Bounds визначає його межі. '
     'Віртуальний деструктор забезпечує правильне delete через Shape*. '
     'Редактор малює та видаляє різні типи через один інтерфейс без перевірок dynamic_cast.')]
for i,(question,answer) in enumerate(answers,1):
    add('h2',f'8.{i} {question}')
    add('p',answer)
add('h1','9 Висновки')
add('p','Я розширив редактор Lab4 до багатовіконної програми Lab5. '
    'Єдиний MyEditor реалізовано через локальний static за патерном Singleton Меєрса. '
    'Немодальна таблиця відображає типи й координати та через callback передає '
    'вибір і видалення без залежності від редактора. Реалізовано текстове збереження '
    'і завантаження всіх шести типів, перевірку пошкоджених даних і збереження попереднього '
    'файлу при невдалій заміні. Збережено успадкування та стилі Lab4. '
    'Усі три бонуси виконано, MinGW-збирання й автоматичні перевірки пройшли.')
add('h1','10 Вихідні тексти власних модулів')
add('p','Наведено головний файл і повні тексти модулів програми, ресурсний шаблон '
    'та конфігурацію CMake. Піктограми bitmap залишено файлами у resources. '
    'Тести та генератор звіту доступні окремо в tests і tools.')
paths=[ROOT/'src/main.cpp']
for name in ['Shape','PointShape','LineShape','RectShape','EllipseShape','CircleLineShape',
             'CubeShape','ShapeFactory','MyEditor','MyTable','MainWindow']:
    paths.extend([ROOT/f'src/{name}.h',ROOT/f'src/{name}.cpp'])
paths.extend([ROOT/'src/GdiGuards.h',ROOT/'resources/resource.h',ROOT/'resources/Lab5.rc.in',ROOT/'CMakeLists.txt'])
for path in paths:
    add('h2',path.relative_to(ROOT).as_posix())
    add('code',path.read_text(encoding='utf-8-sig'))

md=[]
body=[]
for kind,text,extra in blocks:
    if kind=='cover':
        md.append(text+'\n')
        body.append('<section class="cover">'+''.join('<p>'+html.escape(line or '\u00a0')+'</p>' for line in text.splitlines())+'</section>')
    elif kind in ['h1','h2']:
        md.append(('# ' if kind=='h1' else '## ')+text+'\n')
        body.append(f'<{kind}>{html.escape(text)}</{kind}>')
    elif kind=='p':
        md.append(text+'\n')
        body.append('<p>'+html.escape(text)+'</p>')
    elif kind=='code':
        md.append('```\n'+text.rstrip()+'\n```\n')
        body.append('<pre><code>'+html.escape(text.rstrip())+'</code></pre>')
    elif kind=='image':
        md.append(f'![{extra}]({text})\n\n{extra}\n')
        file=(OUT/text).resolve()
        encoded=base64.b64encode(file.read_bytes()).decode()
        body.append('<figure><img src="data:image/png;base64,'+encoded+'" alt="'+html.escape(extra)+'"><figcaption>'+html.escape(extra)+'</figcaption></figure>')

stem='Звіт_Lab5_Варіант_27'
(OUT/(stem+'.md')).write_text('\n'.join(md),encoding='utf-8')
style='''
@page { size: A4; margin: 20mm; }
body { max-width: 900px; margin: 40px auto; color: #111; font: 18px/1.55 "Times New Roman", serif; padding: 0 24px; }
.cover { text-align: center; page-break-after: always; padding: 35px 0; }
.cover p { text-align: center; margin: 9px 0; }
h1 { font-size: 25px; margin-top: 42px; break-after: avoid; }
h2 { font-size: 21px; margin-top: 28px; break-after: avoid; }
p { text-align: justify; }
pre { font: 13px/1.4 Consolas, monospace; white-space: pre-wrap; overflow-wrap: anywhere; padding: 12px; background: #f5f5f5; }
figure { margin: 24px 0; break-inside: avoid; }
img { display: block; max-width: 100%; height: auto; margin: auto; }
figcaption { font-size: 16px; text-align: center; margin-top: 10px; }
@media print { body { margin: 0; padding: 0; font-size: 12pt; } pre { font-size: 8.5pt; background: white; padding: 0; } }
'''
(OUT/(stem+'.html')).write_text('<!doctype html><html lang="uk"><meta charset="utf-8"><title>Звіт Lab5 Варіант 27</title><style>'+style+'</style><body>'+''.join(body)+'</body></html>',encoding='utf-8')

parser=argparse.ArgumentParser()
parser.add_argument('--docx',action='store_true',help='Створити чернетку DOCX у tmp для обов’язкового render QA')
args=parser.parse_args()
if args.docx:
    from docx import Document
    from docx.shared import Inches, Pt, RGBColor
    from docx.enum.text import WD_ALIGN_PARAGRAPH
    doc=Document()
    section=doc.sections[0]
    section.page_width=Inches(8.5)
    section.page_height=Inches(11)
    section.left_margin=section.right_margin=Inches(.8)
    section.top_margin=section.bottom_margin=Inches(.75)
    normal=doc.styles['Normal']
    normal.font.name='Times New Roman'
    normal.font.size=Pt(12)
    normal.paragraph_format.space_after=Pt(6)
    for name in ['Title','Heading 1','Heading 2']:
        doc.styles[name].font.color.rgb=RGBColor(0,0,0)
    for kind,text,extra in blocks:
        if kind=='cover':
            for line in text.splitlines():
                p=doc.add_paragraph(line,style='Title' if line=='ЗВІТ' else 'Normal')
                p.alignment=WD_ALIGN_PARAGRAPH.CENTER
            doc.add_page_break()
        elif kind in ['h1','h2']: doc.add_heading(text,level=1 if kind=='h1' else 2)
        elif kind=='p': doc.add_paragraph(text)
        elif kind=='code':
            for line in text.rstrip().splitlines():
                p=doc.add_paragraph()
                p.paragraph_format.space_after=Pt(0)
                run=p.add_run(line)
                run.font.name='Consolas'
                run.font.size=Pt(9)
        elif kind=='image':
            doc.add_picture(str((OUT/text).resolve()),width=Inches(6.8))
            p=doc.add_paragraph(extra)
            p.alignment=WD_ALIGN_PARAGRAPH.CENTER
    doc.save(ROOT/'tmp/report-unverified.docx')
print('Generated report and UML')
