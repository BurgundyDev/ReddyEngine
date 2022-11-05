#pragma once

#include "GameState.h"

#include <memory>
#include <string>


class ActionManager;
using ActionManagerRef = std::shared_ptr<ActionManager>;


class EditorState final : public GameState
{
public:
    EditorState();

    void update(float dt) override;

private:
    // Events from UI or Shortcuts
    void onNew();
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
    void save(const std::string& filename);

    ActionManagerRef m_pActionManager;
    std::string m_filename;
    bool m_dirty = false;
};
