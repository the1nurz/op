#include "MyEditor.h"
#include "PointShape.h"
#include "LineShape.h"
#include "RectShape.h"
#include "EllipseShape.h"
#include "CircleLineShape.h"
#include "CubeShape.h"
#include <new>

MyEditor::~MyEditor() { Clear(); }

bool MyEditor::SetTool(int commandId) noexcept {
    if (commandId < 10 || commandId > 15) return false;
    Cancel();
    toolId_ = commandId;
    return true;
}

int MyEditor::CurrentTool() const noexcept { return toolId_; }

Shape* MyEditor::CreateShape(int x, int y) const noexcept {
    switch (toolId_) {
    case 10: return new (std::nothrow) PointShape(x, y);
    case 11: return new (std::nothrow) LineShape(x, y);
    case 12: return new (std::nothrow) RectShape(x, y);
    case 13: return new (std::nothrow) EllipseShape(x, y);
    case 14: return new (std::nothrow) CircleLineShape(x, y);
    case 15: return new (std::nothrow) CubeShape(x, y);
    default: return nullptr;
    }
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
}

void MyEditor::Clear() noexcept {
    Cancel();
    for (int i = 0; i < count_; ++i) {
        delete pcshape[i];
        pcshape[i] = nullptr;
    }
    count_ = 0;
}
