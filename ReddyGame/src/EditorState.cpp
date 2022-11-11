#include "EditorState.h"
#include "ActionManager.h"
#include "Game.h"
#include "MainMenuState.h"

#include <Engine/Config.h>
#include <Engine/Event.h>
#include <Engine/Input.h>
#include <Engine/Log.h>
#include <Engine/PFX.h>
#include <Engine/ReddyEngine.h>
#include <Engine/ResourceManager.h>
#include <Engine/Scene.h>
#include <Engine/SpriteBatch.h>
#include <Engine/Utils.h>
#include <Engine/Entity.h>
#include <Engine/Scene.h>
#include <Engine/SpriteComponent.h>

#include <imgui.h>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <glm/gtx/transform.hpp>

#include <SDL_events.h>

static const char *FILE_PATTERNS[] = { "*.json" };


static void addRecentFile(const std::string& filename)
{
    // Remove if already present
    for (auto it = Engine::Config::recentEditorFiles.begin(); it != Engine::Config::recentEditorFiles.end(); ++it)
    {
        if (*it == filename)
        {
            Engine::Config::recentEditorFiles.erase(it);
            break;
        }
    }

    // Put in front
    Engine::Config::recentEditorFiles.insert(Engine::Config::recentEditorFiles.begin(), filename);
}


EditorState::EditorState()
{
    m_pActionManager = std::make_shared<ActionManager>();
}

void EditorState::enter(const GameStateRef& previousState)
{
    Engine::getScene()->setEditorScene(true);

    // Load last recently open
    if (!Engine::Config::recentEditorFiles.empty())
    {
        open(Engine::Config::recentEditorFiles[0]);
    }
    else
    {
        m_filename = "untitled";
        m_pActionManager->clear();
    }

    // Register events
    Engine::getEventSystem()->registerListener<Engine::KeyDownEvent>(this, std::bind(&EditorState::onKeyDown, this, _1));
    Engine::getEventSystem()->registerListener<Engine::MouseButtonDownEvent>(this, std::bind(&EditorState::onMouseDown, this, _1));
}

void EditorState::leave(const GameStateRef& newsState)
{
    Engine::getEventSystem()->deregisterListener<Engine::KeyDownEvent>(this);
    Engine::getEventSystem()->deregisterListener<Engine::MouseButtonDownEvent>(this);
    Engine::getScene()->setEditorScene(false);
}

void EditorState::setDirty(bool dirty)
{
    m_dirty = dirty;
    auto caption = Engine::Utils::getFilenameWithoutExtension(m_filename);
    if (m_dirty) caption += "*";
    Engine::setWindowCaption(caption);
}

void EditorState::onKeyDown(Engine::IEvent* pEvent)
{
    // Handle editor shortcuts
    auto pKeyEvent = (Engine::KeyDownEvent*)pEvent;
    if (pKeyEvent->key.repeat) return; // Ignore repeats

    auto ctrl = (pKeyEvent->key.keysym.mod & KMOD_LCTRL) ? true : false;
    auto shift = (pKeyEvent->key.keysym.mod & KMOD_LSHIFT) ? true : false;
    auto alt = (pKeyEvent->key.keysym.mod & KMOD_LALT) ? true : false;
    auto scancode = pKeyEvent->key.keysym.scancode;

    // Undo/Redo
    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_Z) onUndo();
    if (ctrl && shift && !alt && scancode == SDL_SCANCODE_Z) onRedo();
    
    // Document type specifics
    switch (m_editDocumentType)
    {
        case EditDocumentType::Scene:
        {
            break;
        }
        case EditDocumentType::PFX:
        {
            if (!ctrl && !shift && !alt && scancode == SDL_SCANCODE_SPACE)
                m_pPfxInstance = std::make_shared<Engine::PFXInstance>(m_pPfx);
            break;
        }
    }
}

void EditorState::onMouseDown(Engine::IEvent* pEvent)
{
    auto pDownEvent = (Engine::MouseButtonDownEvent*)pEvent;

    auto ctrl = Engine::getInput()->isKeyDown(SDL_SCANCODE_LCTRL);
    auto shift = Engine::getInput()->isKeyDown(SDL_SCANCODE_LSHIFT);
    auto alt = Engine::getInput()->isKeyDown(SDL_SCANCODE_LALT);

    switch (m_editDocumentType)
    {
        case EditDocumentType::Scene:
        {
            const auto& pHoveredEntity = Engine::getScene()->getHoveredEntity();

            if (!pHoveredEntity && !ctrl && !m_selected.empty())
                changeSelection({}); // Deselect

            if (pHoveredEntity)
            {
                if (!ctrl)
                {
                    changeSelection({pHoveredEntity});
                }
                else
                {
                    auto newSelection = m_selected;
                    newSelection.push_back(pHoveredEntity);
                    changeSelection(newSelection);
                }
            }
            break;
        }
        case EditDocumentType::PFX:
        {
            break;
        }
    }
}

void EditorState::update(float dt)
{
    // Menu bar
    bool quit = false;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::BeginMenu("New"))
            {
                if (ImGui::MenuItem("Scene")) onNew(EditDocumentType::Scene);
                if (ImGui::MenuItem("PFX")) onNew(EditDocumentType::PFX);
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) onOpen();
            if (ImGui::BeginMenu("Open Recent", !Engine::Config::recentEditorFiles.empty()))
            {
                for (const auto& recentFile : Engine::Config::recentEditorFiles)
                {
                    if (ImGui::MenuItem(recentFile.c_str())) open(recentFile);
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) onSave();
            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) onSaveAs();
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Alt+F4"))
            {
                onQuit();
                quit = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", nullptr, m_pActionManager->canUndo())) onUndo();
            if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z", nullptr, m_pActionManager->canRedo())) onRedo();
            ImGui::Separator();
            bool enabled = m_editDocumentType == EditDocumentType::Scene;
            if (ImGui::MenuItem("Cut", "Ctrl+X", nullptr, enabled)) onCut();
            if (ImGui::MenuItem("Copy", "Ctrl+C", nullptr, enabled)) onCopy();
            if (ImGui::MenuItem("Paste", "Ctrl+V", nullptr, enabled)) onPaste();
            if (ImGui::MenuItem("Duplicate", "Ctrl+D", nullptr, enabled)) onDuplicate();
            if (ImGui::MenuItem("Delete", "Del", nullptr, enabled)) onDelete();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Reset Camera")) 
            {
                m_position = {0,0};
                m_zoom = 2;
            }
            ImGui::EndMenu();
        }

        if (m_editDocumentType == EditDocumentType::Scene)
        {
            if (ImGui::BeginMenu("Scene"))
            {
                if (ImGui::BeginMenu("Create Entity (Shift+A)"))
                {
                    if (ImGui::MenuItem("Empty")) onCreateEmptyEntity();
                    if (ImGui::MenuItem("Sprite")) onCreateSpriteEntity();
                    if (ImGui::MenuItem("Text")) onCreateTextEntity();
                    if (ImGui::MenuItem("Sound")) onCreateSoundEntity();
                    if (ImGui::MenuItem("Particle")) onCreateParticleEntity();
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
        }

        if (m_editDocumentType == EditDocumentType::PFX)
        {
            if (ImGui::BeginMenu("PFX"))
            {
                if (ImGui::MenuItem("Play", "Spacebar"))
                {
                    m_pPfxInstance = std::make_shared<Engine::PFXInstance>(m_pPfx);
                }
                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar();
    }

    if (quit) return;

    // Draw UI stuff
    switch (m_editDocumentType)
    {
        case EditDocumentType::Scene:
            drawSceneUI();
            break;
        case EditDocumentType::PFX:
            drawPFXUI();
            if (m_pPfxInstance) m_pPfxInstance->update(dt);
            break;
    }

    // Context menu
    if (m_editDocumentType == EditDocumentType::Scene)
    {
        if (Engine::getInput()->isKeyDown(SDL_SCANCODE_LSHIFT) && Engine::getInput()->isKeyJustDown(SDL_SCANCODE_A))
        {
            ImGui::OpenPopup("CreateEntityContext");
        }
    }

    if (ImGui::BeginPopupContextWindow("CreateEntityContext"))
    {
        if (ImGui::Selectable("Empty")) onCreateEmptyEntity();
        if (ImGui::Selectable("Sprite")) onCreateSpriteEntity();
        if (ImGui::Selectable("Text")) onCreateTextEntity();
        if (ImGui::Selectable("Sound")) onCreateSoundEntity();
        if (ImGui::Selectable("Particle")) onCreateParticleEntity();
        ImGui::EndPopup();
    }

    // Camera stuff
    {
        auto mousePos = Engine::getInput()->getMousePos();
        mousePos -= Engine::getResolution() * 0.5f;
        mousePos /= m_zoomf;
        m_mouseWorldPos = mousePos + m_position;

        // Mouse pan with middle button
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            if (Engine::getInput()->isButtonJustDown(SDL_BUTTON_MIDDLE) || Engine::getInput()->isButtonJustDown(SDL_BUTTON_RIGHT))
            {
                m_mouseOnDown = Engine::getInput()->getMousePos();
                m_positionOnDown = m_position;
            }
            else if (Engine::getInput()->isButtonDown(SDL_BUTTON_MIDDLE) || Engine::getInput()->isButtonDown(SDL_BUTTON_RIGHT))
            {
                auto diff = m_mouseOnDown - Engine::getInput()->getMousePos();
                diff /= m_zoomf;
                m_position = m_positionOnDown + diff;
            }
            else if (Engine::getInput()->getMouseWheel() > 0)
            {
                // Zoom In
                m_zoom = std::max(0, m_zoom - 1);
            }
            else if (Engine::getInput()->getMouseWheel() < 0)
            {
                // Zoom out
                m_zoom = std::min(7, m_zoom + 1);
            }
        }

        // Update zoom
        auto zoomTarget = ZOOM_LEVELS[m_zoom];
        m_zoomf = Engine::Utils::lerp(m_zoomf, zoomTarget, std::min(1.0f, dt * 50.0f));
    }

    // Update scene
    switch (m_editDocumentType)
    {
        case EditDocumentType::Scene:
            Engine::getScene()->setMousePos(m_mouseWorldPos);
            Engine::getScene()->update(dt);
            break;
        case EditDocumentType::PFX:
            break;
    }
}

void EditorState::draw()
{
    glm::mat4 transform = 
        glm::translate(glm::vec3(Engine::getResolution() * 0.5f, 0.0f)) *
        glm::scale(glm::vec3(m_zoomf)) * 
        glm::translate(glm::vec3(-m_position, 0.0f));

    auto sb = Engine::getSpriteBatch().get();
    sb->begin(transform);

    // Draw faint cross in the middle so we know where's the center
    sb->drawRect(nullptr, glm::vec4(-10.0f, 0.0f, 20.0f, 1.0f / m_zoomf), glm::vec4(0.5f));
    sb->drawRect(nullptr, glm::vec4(0.0f, -10.0f, 1.0f / m_zoomf, 20.0f), glm::vec4(0.5f));

    for (int i = -10; i <= 10; ++i)
    {
        sb->drawRect(nullptr, glm::vec4((float)i, -0.1f, 1.0f / m_zoomf, 0.2f), glm::vec4(0.5f));
        sb->drawRect(nullptr, glm::vec4(-0.1f, (float)i, 0.2f, 1.0f / m_zoomf), glm::vec4(0.5f));
    }

    switch (m_editDocumentType)
    {
        case EditDocumentType::Scene:
        {
            Engine::getScene()->draw();

            // Draw gizmos (Selection boxes)
            const glm::vec4 SELECTED_COLOR = { 1, 0, 0, 1 };
            const glm::vec4 HOVER_COLOR = { 1, 1, 0, 1 };

            for (const auto& pEntity : m_selected)
                pEntity->drawOutline(SELECTED_COLOR, 1.0f / m_zoomf);

            auto pHovered = Engine::getScene()->getHoveredEntity();
            if (pHovered && !pHovered->isSelected)
                pHovered->drawOutline(HOVER_COLOR, 1.0f / m_zoomf);

            break;
        }
        case EditDocumentType::PFX:
            if (m_pPfxInstance) m_pPfxInstance->draw({0, 0});
            break;
    }

    sb->end();
}

void EditorState::changeSelection(const std::vector<Engine::EntityRef>& in_newSelection)
{
    auto prevSelection = m_selected;
    auto newSelection = in_newSelection;

    m_pActionManager->doAction("Select",
                               [this, newSelection]() // Redo
    {
        for (const auto& pEntity : m_selected)
            pEntity->isSelected = false;
        m_selected = newSelection;
        for (const auto& pEntity : m_selected)
            pEntity->isSelected = true;
    },
                               [this, prevSelection]() // Undo
    {
        for (const auto& pEntity : m_selected)
            pEntity->isSelected = false;
        m_selected = prevSelection;
        for (const auto& pEntity : m_selected)
            pEntity->isSelected = true;
    });
}


//-----------------------------------------------------------------------
// Actions from commands or shortcuts
//-----------------------------------------------------------------------


void EditorState::onNew(EditDocumentType documentType)
{
    if (!askSaveUnsavedChanges()) return;

    clear();

    m_editDocumentType = documentType;
    if (m_editDocumentType == EditDocumentType::PFX)
    {
        m_pPfx = Engine::PFX::create();
        m_pfxJson = m_pPfx->serialize();
    }
}

void EditorState::onOpen()
{
    if (!askSaveUnsavedChanges()) return;
    openAs();
}

void EditorState::onSave()
{
    //if (!m_dirty) return; // Don't waste processing to save if nothing is changed (We do)
    if (m_filename == "untitled") saveAs(); // Never saved before
    else save();
}

void EditorState::onSaveAs()
{
    saveAs();
}

void EditorState::onQuit()
{
    if (!askSaveUnsavedChanges()) return;

    // Go back to main menu
    g_pGame->changeState(std::make_shared<MainMenuState>());
}

void EditorState::onUndo()
{
    m_pActionManager->undo();
}

void EditorState::onRedo()
{
    m_pActionManager->redo();
}

void EditorState::onCut()
{
}

void EditorState::onCopy()
{
}

void EditorState::onPaste()
{
}

void EditorState::onDuplicate()
{
}

void EditorState::onDelete()
{
}

void EditorState::onCreateEmptyEntity()
{
    auto pEntity = Engine::getScene()->createEntity();
    changeSelection({pEntity});
}

void EditorState::onCreateSpriteEntity()
{
    auto pEntity = Engine::getScene()->createEntity();
    pEntity->addComponent<Engine::SpriteComponent>();
    changeSelection({pEntity});
}

void EditorState::onCreateTextEntity()
{
    CORE_ERROR_POPUP("Unsupported Text Entity yet!");
}

void EditorState::onCreateSoundEntity()
{
    CORE_ERROR_POPUP("Unsupported Sound Entity yet!");
}

void EditorState::onCreateParticleEntity()
{
    CORE_ERROR_POPUP("Unsupported PFX Entity yet!");
}


//-----------------------------------------------------------------------
// File dialog bullshit
//-----------------------------------------------------------------------


void EditorState::open(const std::string& filename)
{
    clear();

    Json::Value json;
    if (!Engine::Utils::loadJson(json, filename))
    {
        CORE_ERROR("Failed to load file: %s", filename.c_str());
        tinyfd_messageBox("Error", ("Failed to load file: " + filename).c_str(), "ok", "error", 0);
        return;
    }

    int version = Engine::Utils::deserializeInt32(json["version"], Engine::FILES_VERSION);
    std::string type = Engine::Utils::deserializeString(json["type"], "none");

    if (type == "scene")
    {
        m_editDocumentType = EditDocumentType::Scene;
        Engine::getScene()->deserialize(json);
    }
    else if (type == "pfx")
    {
        m_editDocumentType = EditDocumentType::PFX;
        m_pPfx = Engine::PFX::createFromJson(json);
        m_pfxJson = m_pPfx->serialize();
    }
    else
    {
        CORE_ERROR("Unknown document type: %s", type.c_str());
        tinyfd_messageBox("Error", ("Unknown document type: " + type).c_str(), "ok", "error", 0);
        return;
    }

    m_position = Engine::Utils::deserializeJsonValue<glm::vec2>(json["camera"]["position"]);
    m_zoom = Engine::Utils::deserializeJsonValue<int>(json["camera"]["zoom"]);
    m_filename = filename;
    m_pActionManager->clear();
    setDirty(false);
    addRecentFile(m_filename);
}

bool EditorState::openAs()
{
    auto filename = tinyfd_openFileDialog("Open", "./assets/scenes/", 1, FILE_PATTERNS, "json files", 0);
    if (!filename) return false;
    
    open(filename);
    return true;
}

void EditorState::save()
{
    setDirty(false);
    Json::Value json;

    switch (m_editDocumentType)
    {
        case EditDocumentType::Scene:
        {
            json = Engine::getScene()->serialize();
            json["type"] = "scene";
            break;
        }
        case EditDocumentType::PFX:
        {
            json = m_pPfx->serialize();
            json["type"] = "pfx";
            break;
        }
    }

    json["version"] = Engine::FILES_VERSION;
    Json::Value cameraJson;
    cameraJson["position"] = Engine::Utils::serializeJsonValue(m_position);
    cameraJson["zoom"] = Engine::Utils::serializeJsonValue(m_zoom);
    json["camera"] = cameraJson;

    Engine::Utils::saveJson(json, m_filename);

    addRecentFile(m_filename);
}

bool EditorState::saveAs()
{
    auto filename = tinyfd_saveFileDialog("Save", "./assets/scenes/", 1, FILE_PATTERNS, "json files");
    if (!filename) return false;

    m_filename = filename;
    if (Engine::Utils::getExtension(m_filename) != "JSON") m_filename += ".json";

    save();
    return true;
}

bool EditorState::askSaveUnsavedChanges()
{
    if (!m_dirty) return true;

    auto result = tinyfd_messageBox("Unsaved Changes", "You have unsaved changes. Do you wish to save?", "yesnocancel", "warning", 0);
    switch (result)
    {
        case 0: // cancel
            return false;
        case 2: // no
            return true;
        case 1: // yes
            if (m_filename == "untitled") // Never saved, do save as
                if (!saveAs()) return false;
            return true;
    }

    return false;
}


//-----------------------------------------------------------------------

void EditorState::clear()
{
    m_filename = "untitled";
    setDirty(true);
    Engine::getScene()->clear();
    m_pActionManager->clear();
    m_pPfxInstance.reset();
    m_pPfx.reset();
    m_position = {0, 0};
    m_zoom = 2;
    m_zoomf = ZOOM_LEVELS[2];
}
