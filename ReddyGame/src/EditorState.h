#pragma once

#include "GameState.h"

#include <json/json.h>
#include <glm/vec2.hpp>

#include <memory>
#include <string>


class ActionManager;
using ActionManagerRef = std::shared_ptr<ActionManager>;

namespace Engine
{
    class PFX;
    using PFXRef = std::shared_ptr<PFX>;

    class PFXInstance;
    using PFXInstanceRef = std::shared_ptr<PFXInstance>;
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
    void draw() override;

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

    void drawSceneUI();
    void drawPFXUI();

    EditDocumentType m_editDocumentType = EditDocumentType::Scene;
    ActionManagerRef m_pActionManager;
    std::string m_filename = "untitled";
    bool m_dirty = true;

    //TODO: pan around and zoom
    glm::vec2 m_position = {2.0f, 0};
    int m_zoom = 5; // Maps to a zoom level
    float m_zoomf = 128.0f;

    Json::Value m_pfxJson; // Last serialized since last modification
    Engine::PFXRef m_pPfx;
    Engine::PFXInstanceRef m_pPfxInstance;
};
