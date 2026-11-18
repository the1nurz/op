#include <cassert>
#include "MyEditor.h"
#include <type_traits>
#include <filesystem>
#include <fstream>
#include <vector>

static_assert(!std::is_default_constructible<MyEditor>::value, "Private constructor");
static_assert(!std::is_copy_constructible<MyEditor>::value, "No copies");
static_assert(!std::is_move_constructible<MyEditor>::value, "No moves");
static_assert(!std::is_copy_assignable<MyEditor>::value, "No assignments");
static_assert(!std::is_destructible<MyEditor>::value, "Only singleton controls lifetime");

static_assert(MyEditor::kCapacity == 128, "Capacity for variant 27");

int main() {
    MyEditor& editor = MyEditor::Instance();
    editor.Clear();
    assert(&editor == &MyEditor::Instance());
    assert(editor.Count() == 0);
    assert(editor.CurrentTool() == 10);

    for (int toolId = 10; toolId <= 15; ++toolId) {
        assert(editor.SetTool(toolId));
        assert(editor.CurrentTool() == toolId);
        assert(editor.Begin(toolId * 10, toolId * 10));
        assert(editor.HasDraft());
        editor.Update(toolId * 10 + 5, toolId * 10 + 8);
        assert(editor.Commit());
        assert(!editor.HasDraft());
        assert(editor.Count() == toolId - 9);
    }

    assert(!editor.SetTool(9));
    assert(!editor.SetTool(16));
    assert(!editor.Commit());
    assert(editor.Begin(0, 0));
    assert(!editor.Begin(1, 1));
    assert(editor.Cancel());
    assert(!editor.HasDraft());
    assert(editor.Count() == 6);

    assert(editor.Begin(10, 10));
    assert(editor.SetTool(10));
    assert(!editor.HasDraft());
    assert(editor.Count() == 6);

    editor.Clear();
    assert(editor.Count() == 0);
    assert(!editor.HasDraft());

    for (int i = 0; i < MyEditor::kCapacity; ++i) {
        assert(editor.SetTool(10 + i % 6));
        assert(editor.Begin(i, i));
        assert(editor.Commit());
    }
    assert(editor.Count() == 128);
    assert(!editor.Begin(129, 129));
    assert(editor.Count() == 128);

    editor.Clear();
    assert(editor.Count() == 0);

    const auto directory = std::filesystem::temp_directory_path()
        / (L"Lab5-test-" + std::to_wstring(GetCurrentProcessId()));
    std::filesystem::create_directory(directory);
    const auto saved = directory / L"фігури.txt";
    const auto broken = directory / L"bad.txt";
    std::wstring error;
    for (int id = 10; id <= 15; ++id) {
        assert(editor.SetTool(id));
        assert(editor.Begin(id*7, -id*3));
        editor.Update(id*9, id*2);
        assert(editor.Commit());
    }
    assert(editor.Save(saved.wstring(), error));
    assert(error.empty());
    editor.Clear();
    assert(editor.Load(saved.wstring(), error));
    assert(editor.Count() == 6);
    for (int i = 0; i < 6; ++i) {
        const Shape* shape = editor.At(i);
        assert(shape->TypeId() == i+10);
        assert(shape->StartX() == (i+10)*7 && shape->StartY() == -(i+10)*3);
        assert(shape->EndX() == (i+10)*9 && shape->EndY() == (i+10)*2);
    }
    assert(!editor.At(-1) && !editor.At(6));
    editor.Select(4);
    const Shape* original = editor.At(0);
    const std::vector<std::string> invalid = {
        "", "LAB5 2 0", "LAB5 1 -1", "LAB5 1 129", "OTHER 1 0",
        "LAB5 1 1\n99 0 0 10 10", "LAB5 1 1\n10 0 0",
        "LAB5 1 1\n10 0 0 2147483648 0", "LAB5 1 1\n10 0 0 1000001 0",
        "LAB5 1 0\nextra", "LAB5 1 1\n10 0 0 1 1\n12 0 0 2 2",
        std::string(65537, 'x')
    };
    for (const auto& text : invalid) {
        { std::ofstream out(broken, std::ios::binary); out << text; }
        assert(!editor.Load(broken.wstring(), error) && !error.empty());
        assert(editor.Count() == 6 && editor.At(0) == original && editor.Selected() == 4);
    }
    assert(!editor.Load((directory/L"missing.txt").wstring(), error));
    // Заборона заміни відкритого файлу не має знищувати його вміст.
    HANDLE lock = CreateFileW(saved.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    assert(lock != INVALID_HANDLE_VALUE);
    assert(editor.Delete(0));
    assert(editor.Selected() == 3 && editor.At(0)->TypeId() == 11);
    assert(!editor.Save(saved.wstring(), error));
    CloseHandle(lock);
    assert(editor.Load(saved.wstring(), error) && editor.Count() == 6);
    assert(!editor.Save((directory/L"missing"/L"file.txt").wstring(), error));
    editor.Select(5);
    assert(editor.Delete(5) && editor.Selected() == -1);
    assert(!editor.Delete(-1) && !editor.Delete(5));
    assert(editor.Delete(2) && editor.At(2)->TypeId() == 13);
    editor.Clear();
    assert(editor.Save(saved.wstring(), error));
    assert(editor.Load(saved.wstring(), error) && editor.Count() == 0);
    for (int i = 0; i < 128; ++i) {
        assert(editor.SetTool(10 + i%6));
        assert(editor.Begin(i, i));
        editor.Update(-i, -i);
        assert(editor.Commit());
    }
    assert(editor.Save(saved.wstring(), error));
    editor.Clear();
    assert(editor.Load(saved.wstring(), error) && editor.Count() == 128);
    assert(!editor.Begin(0, 0));
    editor.Clear();
    std::filesystem::remove_all(directory);
    return 0;
}
