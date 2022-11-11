#pragma once

#include "GameState.h"

#include <Engine/Event.h>

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

    class Entity;
    using EntityRef = std::shared_ptr<Entity>;

    class IEvent;
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
    void drawEntitySceneTree(const Engine::EntityRef& pEntity);
    const char* getEntityFriendlyName(const Engine::EntityRef& pEntity);

    void onKeyDown(Engine::IEvent* pEvent);

    EditDocumentType m_editDocumentType = EditDocumentType::Scene;
    ActionManagerRef m_pActionManager;
    std::string m_filename = "untitled";
    bool m_dirty = true;

    // Mouse stuff
    glm::vec2 m_mouseWorldPos;

    // Camera stuff
    glm::vec2 m_mouseOnDown;
    glm::vec2 m_positionOnDown;
    glm::vec2 m_position = {0, 0};
    const float ZOOM_LEVELS[8] = {256.0f, 192.0f, 128.0f, 96.0f, 64.0f, 32.0f, 16.0f, 8.0f};
    int m_zoom = 2; // Maps to a zoom level
    float m_zoomf = ZOOM_LEVELS[2];

    // Particle editor stuff
    Json::Value m_pfxJson; // Last serialized since last modification
    Engine::PFXRef m_pPfx;
    Engine::PFXInstanceRef m_pPfxInstance;

    // Scene editor stuff
    std::vector<Engine::EntityRef> m_selected;
};
