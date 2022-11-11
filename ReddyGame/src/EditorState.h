#pragma once

#include "GameState.h"

#include <Engine/Event.h>
#include <Engine/Entity.h>

#include <json/json.h>
#include <glm/vec2.hpp>

#include <memory>
#include <string>
#include <glm/vec4.hpp>


class ActionManager;
using ActionManagerRef = std::shared_ptr<ActionManager>;

namespace Engine
{
    class PFX;
    using PFXRef = std::shared_ptr<PFX>;

    class PFXInstance;
    using PFXInstanceRef = std::shared_ptr<PFXInstance>;

    class IEvent;
}


enum class EditDocumentType
{
    Scene,
    PFX
};


enum class TransformType
{
    None,
    Translate,
    Rotate,
    Scale
};


class EditorState final : public GameState
{
public:
    EditorState();

    void enter(const GameStateRef& previousState) override;
    void leave(const GameStateRef& newsState) override;
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
    void onCreateEmptyEntity();
    void onCreateSpriteEntity();
    void onCreateTextEntity();
    void onCreateSoundEntity();
    void onCreateParticleEntity();

    template<typename T>
    void onAddComponent()
    {
        if (m_selected.size() != 1) return;
        if (m_selected.front()->hasComponent<T>()) return; // Do nothing
        m_selected.front()->addComponent<T>();
        pushUndo("Add Component");
    }

    void open(const std::string& filename);
    bool openAs(); // Returns false if user cancelled
    void save();
    bool saveAs(); // Returns false if user cancelled
    bool askSaveUnsavedChanges(); // Returns false if operation was cancelled

    void drawSceneUI();
    void drawPFXUI();
    void setDirty(bool dirty);
    void clear();
    void changeSelection(const std::vector<Engine::EntityRef>& newSelection);
    void changeSelectionAction(const std::vector<Engine::EntityRef>& newSelection);
    void drawEntitySceneTree(const Engine::EntityRef& pEntity);
    const char* getEntityFriendlyName(const Engine::EntityRef& pEntity);
    void serializeSelectionState();
    void updateTransform();
    void createEntityAction(Engine::EntityRef pEntity);

    void onKeyDown(Engine::IEvent* pEvent);
    void onKeyUp(Engine::IEvent* pEvent);
    void onMouseDown(Engine::IEvent* pEvent);
    void onMouseUp(Engine::IEvent* pEvent);
    void onDropEvent(Engine::IEvent* pEvent);

    void pushUndo(const char* actionName);

    EditDocumentType m_editDocumentType = EditDocumentType::Scene;
    ActionManagerRef m_pActionManager;
    std::string m_filename = "untitled";
    bool m_dirty = true;
    bool m_openCreateEntityMenu = false;
    Uint8 m_mouseButton = 0;

    bool m_shiftHeld = false;
    bool m_altHeld = false;
    bool m_ctrlHeld = false;

    // Mouse stuff
    glm::vec2 m_mouseWorldPos;

    // Camera stuff
    glm::vec2 m_mouseOnDown;
    glm::vec2 m_mouseScreenOnDown;
    glm::vec2 m_positionOnDown;
    glm::vec2 m_position = {0, 0};
    const float ZOOM_LEVELS[8] = {256.0f, 192.0f, 128.0f, 96.0f, 64.0f, 32.0f, 16.0f, 8.0f};
    int m_zoom = 2; // Maps to a zoom level
    float m_zoomf = ZOOM_LEVELS[2];
    float m_gridHideZoomLevel = ZOOM_LEVELS[5];

    // Grid stuff
    bool m_isGridVisible = true;
    glm::vec2 m_gridStep = { 8.0f, 8.0f };
    glm::vec4 m_gridColor = glm::vec4(0.35f);

    // Particle editor stuff
    Json::Value m_pfxJson; // Last serialized since last modification
    Engine::PFXRef m_pPfx;
    Engine::PFXInstanceRef m_pPfxInstance;

    // Scene editor stuff
    Json::Value m_prevJson;
    std::vector<uint64_t> m_selectedIds;

    std::vector<Engine::EntityRef> m_selected;
    std::vector<glm::vec2> m_worldPositionsOnDown;
    TransformType m_transformType = TransformType::None;
    bool m_isMouseDownInWorld = false;
    bool m_wasAddedToSelection = false; // Another gross hack

    float m_snapScale = 0.5f;
};
