#pragma once

#include "GameState.h"

#include <memory>
#include <string>


class ActionManager;
using ActionManagerRef = std::shared_ptr<ActionManager>;

namespace Engine
{
    class PFX;
    using PFXRef = std::shared_ptr<PFX>;
}


enum class EditDocumentType
{
    Scene,
    PFX
};


class EditorState final : public GameState
{
public:
    EditorState();

    void enter(const GameStateRef& previousState) override;
    void update(float dt) override;

private:
    // Events from UI or Shortcuts
    void onNew(EditDocumentType documentType);
    void onOpen();
    void onSave();
    void onSaveAs();
    void onQuit();
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDuplicate();
    void onDelete();

    void open(const std::string& filename);
    bool openAs(); // Returns false if user cancelled
    void save();
    bool saveAs(); // Returns false if user cancelled
    bool askSaveUnsavedChanges(); // Returns false if operation was cancelled

    EditDocumentType m_editDocumentType = EditDocumentType::Scene;
    ActionManagerRef m_pActionManager;
    std::string m_filename = "untitled";
    bool m_dirty = true;
    Engine::PFXRef m_pPfx;
};
