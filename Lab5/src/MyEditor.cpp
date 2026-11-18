#include "MyEditor.h"
#include "ShapeFactory.h"
#include "GdiGuards.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <locale>
#include <new>

MyEditor& MyEditor::Instance() noexcept {
    static MyEditor instance;
    return instance;
}

MyEditor::~MyEditor() { Clear(); }

bool MyEditor::SetTool(int commandId) noexcept {
    if (!FindShapeType(commandId)) return false;
    Cancel();
    toolId_ = commandId;
    return true;
}

int MyEditor::CurrentTool() const noexcept { return toolId_; }

Shape* MyEditor::CreateShape(int x, int y) const noexcept {
    return CreateShapeByType(toolId_, x, y);
}

bool MyEditor::Begin(int x, int y) noexcept {
    if (draft_ || count_ >= kCapacity) return false;
    draft_ = CreateShape(x, y);
    return draft_ != nullptr;
}

void MyEditor::Update(int x, int y) noexcept {
    if (draft_) draft_->SetEnd(x, y);
}

bool MyEditor::Commit() noexcept {
    if (!draft_ || count_ >= kCapacity) return false;
    pcshape[count_++] = draft_;
    draft_ = nullptr;
    return true;
}

bool MyEditor::Cancel() noexcept {
    if (!draft_) return false;
    delete draft_;
    draft_ = nullptr;
    return true;
}

bool MyEditor::HasDraft() const noexcept { return draft_ != nullptr; }
int MyEditor::Count() const noexcept { return count_; }

void MyEditor::Show(HDC dc) const {
    if (!dc) return;
    for (int i = 0; i < count_; ++i) {
        if (pcshape[i]) pcshape[i]->Show(dc);
    }
    if (draft_) draft_->Show(dc, true);
    if (const Shape* shape = At(selected_)) {
        RECT r = shape->Bounds();
        InflateRect(&r, 5, 5);
        ScopedGdiObject pen(CreatePen(PS_DOT, 1, RGB(0, 90, 210)));
        if (!pen.Get()) return;
        ScopedGdiSelection selectedPen(dc, pen.Get());
        ScopedGdiSelection brush(dc, GetStockObject(NULL_BRUSH));
        if (!selectedPen.IsSelected() || !brush.IsSelected()) return;
        const int oldMode = SetBkMode(dc, TRANSPARENT);
        Rectangle(dc, r.left, r.top, r.right, r.bottom);
        SetBkMode(dc, oldMode);
    }
}

void MyEditor::Clear() noexcept {
    Cancel();
    for (int i = 0; i < count_; ++i) {
        delete pcshape[i];
        pcshape[i] = nullptr;
    }
    count_ = 0;
    selected_ = -1;
}

const Shape* MyEditor::At(int index) const noexcept {
    return index >= 0 && index < count_ ? pcshape[index] : nullptr;
}

void MyEditor::Select(int index) noexcept { selected_ = At(index) ? index : -1; }

bool MyEditor::Delete(int index) noexcept {
    if (!At(index)) return false;
    delete pcshape[index];
    for (int i = index; i < count_-1; ++i) pcshape[i] = pcshape[i+1];
    pcshape[--count_] = nullptr;
    if (selected_ == index) selected_ = -1;
    else if (selected_ > index) --selected_;
    return true;
}

bool MyEditor::Save(const std::wstring& path, std::wstring& error) const {
    error.clear();
    try {
        std::ostringstream text;
        text.imbue(std::locale::classic());
        text << "LAB5 1 " << count_ << '\n';
        for (int i = 0; i < count_; ++i) {
            const Shape& s = *pcshape[i];
            text << s.TypeId() << ' ' << s.StartX() << ' ' << s.StartY() << ' '
                << s.EndX() << ' ' << s.EndY() << '\n';
        }
        const std::string data = text.str();
        const auto target = std::filesystem::absolute(path);
        wchar_t temp[MAX_PATH]{};
        // Тимчасовий файл у тій самій папці, щоб заміна була в межах одного тому.
        if (!GetTempFileNameW(target.parent_path().c_str(), L"lb5", 0, temp)) {
            error = L"Не вдалося створити тимчасовий файл у вибраній папці.";
            return false;
        }
        HANDLE file = CreateFileW(temp, GENERIC_WRITE, 0, nullptr, TRUNCATE_EXISTING,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        DWORD written = 0;
        bool ok = file != INVALID_HANDLE_VALUE;
        if (ok) {
            ok = WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &written, nullptr)
                && written == data.size();
            if (ok) ok = FlushFileBuffers(file) != FALSE;
            if (!CloseHandle(file)) ok = false;
        }
        if (ok) ok = MoveFileExW(temp, target.c_str(),
            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE;
        if (!ok) {
            DeleteFileW(temp);
            error = L"Не вдалося записати або замінити файл. Попередній файл збережено.";
        }
        return ok;
    } catch (const std::exception&) {
        error = L"Помилка підготовки даних або шляху до файлу.";
        return false;
    }
}

bool MyEditor::Load(const std::wstring& path, std::wstring& error) {
    error.clear();
    try {
        std::ifstream file(std::filesystem::path(path), std::ios::binary | std::ios::ate);
        if (!file || file.tellg() < 0 || file.tellg() > 65536) {
            error = L"Не вдалося відкрити файл або його розмір перевищує 64 КіБ.";
            return false;
        }
        const auto size = static_cast<std::streamsize>(file.tellg());
        file.seekg(0);
        std::string data(static_cast<size_t>(size), '\0');
        // Читаємо рівно перевірений розмір, відхиляємо зміну файлу під час читання.
        if (!file.read(data.data(), size) || file.peek() != std::char_traits<char>::eof()) {
            error = L"Помилка читання або файл змінився під час читання.";
            return false;
        }
        std::istringstream input(data);
        input.imbue(std::locale::classic());
        std::string magic;
        int version = 0, count = 0;
        if (!(input >> magic >> version >> count) || magic != "LAB5" || version != 1
            || count < 0 || count > kCapacity) {
            error = L"Некоректний заголовок файлу LAB5 або кількість об’єктів.";
            return false;
        }
        std::vector<std::unique_ptr<Shape>> loaded;
        for (int i = 0; i < count; ++i) {
            int type, x1, y1, x2, y2;
            if (!(input >> type >> x1 >> y1 >> x2 >> y2)
                || x1 < -1000000 || x1 > 1000000 || y1 < -1000000 || y1 > 1000000
                || x2 < -1000000 || x2 > 1000000 || y2 < -1000000 || y2 > 1000000) {
                error = L"Пошкоджені координати або неповний опис об’єкта.";
                return false;
            }
            std::unique_ptr<Shape> shape(CreateShapeByType(type, x1, y1));
            if (!shape) { error = L"Невідомий тип фігури або недостатньо пам’яті."; return false; }
            shape->SetEnd(x2, y2);
            loaded.push_back(std::move(shape));
        }
        input >> std::ws;
        if (!input.eof()) { error = L"Зайві дані після останнього об’єкта."; return false; }
        // Лише повністю перевірений набір замінює поточний малюнок.
        Clear();
        for (auto& shape : loaded) pcshape[count_++] = shape.release();
        return true;
    } catch (const std::exception&) {
        error = L"Помилка читання файлу або недостатньо пам’яті.";
        return false;
    }
}
