#include <cassert>
#include "MyEditor.h"

int main() {
    MyEditor editor;
    assert(editor.Count() == 0);
    assert(editor.CurrentTool() == 10);

    for (int toolId = 10; toolId <= 13; ++toolId) {
        assert(editor.SetTool(toolId));
        assert(editor.CurrentTool() == toolId);
        assert(editor.Begin(toolId * 10, toolId * 10));
        assert(editor.HasDraft());
        editor.Update(toolId * 10 + 5, toolId * 10 + 8);
        assert(editor.Commit());
        assert(!editor.HasDraft());
        assert(editor.Count() == toolId - 9);
    }

    assert(editor.Begin(0, 0));
    assert(editor.Cancel());
    assert(!editor.HasDraft());
    assert(editor.Count() == 4);

    editor.Clear();
    assert(editor.Count() == 0);
    assert(!editor.HasDraft());

    for (int i = 0; i < MyEditor::kCapacity; ++i) {
        assert(editor.SetTool(10));
        assert(editor.Begin(i, i));
        assert(editor.Commit());
    }
    assert(editor.Count() == 128);
    assert(!editor.Begin(129, 129));
    assert(editor.Count() == 128);

    editor.Clear();
    assert(editor.Count() == 0);
    return 0;
}
