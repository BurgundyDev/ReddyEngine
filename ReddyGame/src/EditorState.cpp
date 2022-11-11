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
#include <Engine/Entity.h>

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

    clear();

    // Load last recently open
    if (!Engine::Config::recentEditorFiles.empty())
    {
        open(Engine::Config::recentEditorFiles[0]);
    }

    // Register events
    Engine::getEventSystem()->registerListener<Engine::KeyDownEvent>(this, std::bind(&EditorState::onKeyDown, this, _1));
    Engine::getEventSystem()->registerListener<Engine::MouseButtonDownEvent>(this, std::bind(&EditorState::onMouseDown, this, _1));
    Engine::getEventSystem()->registerListener<Engine::MouseButtonUpEvent>(this, std::bind(&EditorState::onMouseUp, this, _1));
}

void EditorState::leave(const GameStateRef& newsState)
{
    Engine::getEventSystem()->deregisterListener<Engine::KeyDownEvent>(this);
    Engine::getEventSystem()->deregisterListener<Engine::MouseButtonUpEvent>(this);
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
            if (!ctrl && shift && !alt && scancode == SDL_SCANCODE_A)
                m_openCreateEntityMenu = true;
            if (!ctrl && !shift && !alt && scancode == SDL_SCANCODE_DELETE)
                onDelete();
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
    
    // Transforming with mouse
    updateTransform();

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


    glm::vec2 resolution = Engine::getResolution();
    glm::vec2 resolutionRatio = glm::vec2(resolution.x / resolution.y, resolution.y / resolution.x);

    const glm::vec4 MID_GRID_COLOR(0.7f);

    if (m_isGridVisible && m_zoomf >= m_gridHideZoomLevel)
    {
        glm::vec2 realGridOffset = glm::vec2( fmod(m_position.x, 1.0f), fmod(m_position.y, 1.0f));

        float maxVal = resolution.x > resolution.y ? resolution.x : resolution.y;

        float gridStart = maxVal / -m_zoomf - 2.0f;
        float gridEnd = maxVal / m_zoomf + 2.0f;

        for (int i = (int) gridStart; i < gridEnd; i++)
        {
            const glm::vec2 cell = glm::vec2(m_position.x + i - realGridOffset.x , m_position.y + gridStart / 2 - realGridOffset.y);

            sb->drawRect(nullptr, glm::vec4(cell.x, cell.y, 1.0f / m_zoomf, gridEnd), (i == 0) ? MID_GRID_COLOR : m_gridColor);
        }

		for (int i = (int) gridStart; i < gridEnd; i++)
		{
	        const glm::vec2 cell = glm::vec2(m_position.x + gridStart / 2 - realGridOffset.x, m_position.y + i - realGridOffset.y);

		    sb->drawRect(nullptr, glm::vec4(cell.x, cell.y, gridEnd, 1.0f / m_zoomf), (i == 0) ? MID_GRID_COLOR : m_gridColor);
		}
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

void EditorState::serializeSelectionState()
{
     for (const auto& pEntity : m_selected)
     {
         pEntity->undoJson = pEntity->serialize(false);
     }
}

void EditorState::changeSelection(const std::vector<Engine::EntityRef>& in_newSelection)
{
    for (const auto& pEntity : m_selected)
        pEntity->isSelected = false;

    m_selected = in_newSelection;

    for (const auto& pEntity : m_selected)
        pEntity->isSelected = true;

    serializeSelectionState();
}

void EditorState::changeSelectionAction(const std::vector<Engine::EntityRef>& in_newSelection)
{
    changeSelection(in_newSelection);
    pushUndo("Select");
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

void EditorState::pushUndo(const char* actionName)
{
    std::vector<uint64_t> ids;
    for (const auto& pEntity : m_selected)
    {
        ids.push_back(pEntity->id);
    }

    const auto& pRoot = Engine::getScene()->getRoot();

    auto jsonBefore = m_prevJson;
    auto jsonAfter = pRoot->serialize(true);

    auto selectedBefore = m_selectedIds;
    auto selectedAfter = ids;

    m_selectedIds = ids;
    m_prevJson = jsonAfter;
    setDirty(true);

    m_pActionManager->addAction(actionName,
        [this, jsonAfter, selectedAfter]() // Redo
        {
            Engine::getScene()->getRoot()->deserialize(jsonAfter, true);

            std::vector<Engine::EntityRef> newSelection;
            for (auto id : selectedAfter)
            {
                auto pEntity = Engine::getScene()->findEntity(id);
                if (pEntity)
                    newSelection.push_back(pEntity);
            }
            changeSelection(newSelection);

            m_selectedIds = selectedAfter;

            setDirty(true);
        },
        [this, jsonBefore, selectedBefore]() // Undo
        {
            Engine::getScene()->getRoot()->deserialize(jsonBefore, true);

            std::vector<Engine::EntityRef> newSelection;
            for (auto id : selectedBefore)
            {
                auto pEntity = Engine::getScene()->findEntity(id);
                if (pEntity)
                    newSelection.push_back(pEntity);
            }
            changeSelection(newSelection);

            m_selectedIds = selectedBefore;

            setDirty(true);
        });
}

void EditorState::onDelete()
{
    if (m_selected.empty()) return;

    std::vector<uint64_t> ids;
    const auto& pRoot = Engine::getScene()->getRoot();
    for (const auto& pEntity : m_selected)
    {
        if (pEntity == pRoot)
        {
            CORE_ERROR_POPUP("Cannot delete Root Entity. Make sure it's not selected before deleting.");
            return;
        }
        ids.push_back(pEntity->id);
    }

    for (auto id : ids)
        Engine::getScene()->destroyEntity(id);
    changeSelection({});

    pushUndo("Delete");
}

void EditorState::createEntityAction(Engine::EntityRef pEntity)
{
    changeSelection({pEntity});
    pushUndo("Create Entity");
}

void EditorState::onCreateEmptyEntity()
{
    auto pEntity = Engine::getScene()->createEntity();
    createEntityAction(pEntity);
}

void EditorState::onCreateSpriteEntity()
{
    auto pEntity = Engine::getScene()->createEntity();
    pEntity->addComponent<Engine::SpriteComponent>();
    createEntityAction(pEntity);
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
        m_prevJson = json["root"];
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
    m_prevJson = Json::Value();
    m_selectedIds.clear();
    m_selected.clear();
}
