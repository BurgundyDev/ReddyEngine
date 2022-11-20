#include "EditorState.h"
#include "ActionManager.h"
#include "Game.h"
#include "MainMenuState.h"

#include <Engine/Component.h>
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
#include <Engine/MusicManager.h>
#include <Engine/LuaBindings.h>
#include <Engine/PFXComponent.h>

#include <imgui.h>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <glm/gtx/transform.hpp>

#include <SDL_events.h>
#include <SDL.h>

#include "Engine/Constants.h"

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
    : GameState("")
{
    m_pActionManager = std::make_shared<ActionManager>();
}

void EditorState::enter(const GameStateRef& previousState)
{
    Engine::getMusicManager()->stop();
    Engine::getScene()->setEditorScene(true);

    clear();
    Engine::getLuaBindings()->init();

    // Load last recently open
    if (!Engine::Config::recentEditorFiles.empty())
    {
        open(Engine::Config::recentEditorFiles[0]);
    }

    // Register events
    Engine::getEventSystem()->registerListener<Engine::KeyDownEvent>(this, std::bind(&EditorState::onKeyDown, this, _1));
    Engine::getEventSystem()->registerListener<Engine::KeyUpEvent>(this, std::bind(&EditorState::onKeyUp, this, _1));
    Engine::getEventSystem()->registerListener<Engine::MouseButtonDownEvent>(this, std::bind(&EditorState::onMouseDown, this, _1));
    Engine::getEventSystem()->registerListener<Engine::MouseButtonUpEvent>(this, std::bind(&EditorState::onMouseUp, this, _1));
    Engine::getEventSystem()->registerListener<Engine::DropEvent>(this, std::bind(&EditorState::onDropEvent, this, _1));
}

void EditorState::leave(const GameStateRef& newsState)
{
    Engine::getEventSystem()->deregisterListener<Engine::KeyDownEvent>(this);
    Engine::getEventSystem()->deregisterListener<Engine::KeyUpEvent>(this);
    Engine::getEventSystem()->deregisterListener<Engine::MouseButtonDownEvent>(this);
    Engine::getEventSystem()->deregisterListener<Engine::MouseButtonUpEvent>(this);
    Engine::getEventSystem()->deregisterListener<Engine::DropEvent>(this);
    Engine::getScene()->setEditorScene(false);

    Engine::Component::clearCachedEditorIcons();

    clear();
    Engine::getLuaBindings()->clear();
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
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    // Handle editor shortcuts
    auto pKeyEvent = (Engine::KeyDownEvent*)pEvent;
    if (pKeyEvent->key.repeat) return; // Ignore repeats

    auto ctrl = (pKeyEvent->key.keysym.mod & KMOD_LCTRL) ? true : false;
    auto shift = (pKeyEvent->key.keysym.mod & KMOD_LSHIFT) ? true : false;
    auto alt = (pKeyEvent->key.keysym.mod & KMOD_LALT) ? true : false;
    auto scancode = pKeyEvent->key.keysym.scancode;

    m_shiftHeld |= shift;
    m_altHeld |= alt;
    m_ctrlHeld |= ctrl;

    // Undo/Redo
    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_Z) onUndo();
    if (ctrl && shift && !alt && scancode == SDL_SCANCODE_Z) onRedo();

    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_O) onOpen();
    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_S) onSave();
    if (ctrl && shift && !alt && scancode == SDL_SCANCODE_S) onSaveAs();

    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_G) onDisableGrid();
    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_H) onDisableViewportOutline();
    
    // Document type specifics
    switch (m_editDocumentType)
    {
        case EditDocumentType::Scene:
        {
            if (!ctrl && shift && !alt && scancode == SDL_SCANCODE_A) m_openCreateEntityMenu = true;
            if (!ctrl && !shift && !alt && scancode == SDL_SCANCODE_DELETE) onDelete();
            if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_X) onCut();
            if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_C) onCopy();
            if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_V) onPaste();
            if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_D) onDuplicate();
            if (!ctrl && !shift && !alt && scancode == SDL_SCANCODE_ESCAPE) onDeselect();
            if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_A) onSelectAll();
            break;
        }
        case EditDocumentType::PFX:
        {
            if (!ctrl && !shift && !alt && scancode == SDL_SCANCODE_SPACE)
                m_pPfxInstance = std::make_shared<Engine::PFXInstance>(m_pPfx);
            break;
        }
    }

    // Nudge selection
    const float NUDGE_AMOUNT = 2.0f / 128.0f;
    const float LARGE_NUDGE_AMOUNT = 16.0f / 128.0f;
    if (!ctrl && !shift && !alt && scancode == SDL_SCANCODE_UP) nudgeSelection(glm::vec2(0, -NUDGE_AMOUNT));
    if (!ctrl && !shift && !alt && scancode == SDL_SCANCODE_DOWN) nudgeSelection(glm::vec2(0, NUDGE_AMOUNT));
    if (!ctrl && !shift && !alt && scancode == SDL_SCANCODE_LEFT) nudgeSelection(glm::vec2(-NUDGE_AMOUNT, 0.0f));
    if (!ctrl && !shift && !alt && scancode == SDL_SCANCODE_RIGHT) nudgeSelection(glm::vec2(NUDGE_AMOUNT, 0.0f));
    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_UP) nudgeSelection(glm::vec2(0, -LARGE_NUDGE_AMOUNT));
    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_DOWN) nudgeSelection(glm::vec2(0, LARGE_NUDGE_AMOUNT));
    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_LEFT) nudgeSelection(glm::vec2(-LARGE_NUDGE_AMOUNT, 0.0f));
    if (ctrl && !shift && !alt && scancode == SDL_SCANCODE_RIGHT) nudgeSelection(glm::vec2(LARGE_NUDGE_AMOUNT, 0.0f));
}

void EditorState::nudgeSelection(const glm::vec2& amount)
{
    if (m_selected.empty()) return;
    for (const auto& pEntity : m_selected)
        pEntity->setWorldPosition(pEntity->getWorldPosition() + amount);
    pushUndo("Nudge");
}

void EditorState::onKeyUp(Engine::IEvent* pEvent)
{
    // Handle editor shortcuts
    auto pKeyEvent = (Engine::KeyUpEvent*)pEvent;
    if (pKeyEvent->key.repeat) return; // Ignore repeats

    auto ctrl = (pKeyEvent->key.keysym.mod & KMOD_LCTRL) ? true : false;
    auto shift = (pKeyEvent->key.keysym.mod & KMOD_LSHIFT) ? true : false;
    auto alt = (pKeyEvent->key.keysym.mod & KMOD_LALT) ? true : false;
    
    m_shiftHeld = shift;
    m_altHeld = alt;
    m_ctrlHeld = ctrl;
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
            ImGui::Separator();
            if (ImGui::MenuItem("Select All", "Ctrl+A", nullptr, enabled)) onSelectAll();
            if (ImGui::MenuItem("Deselect", "Escape", nullptr, enabled)) onDeselect();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Reset Camera")) 
            {
                m_positionTarget = m_position = {0,0};
                m_zoom = 2;
            }

            if(m_isGridVisible)
            {
                if (ImGui::MenuItem("Disable Grid", "Ctrl + G", nullptr, true)) onDisableGrid();
            } else
            {
                if (ImGui::MenuItem("Enable Grid", "Ctrl + G", nullptr, true)) onDisableGrid();
            }

            if (m_isViewportOutlined)
            {
                if (ImGui::MenuItem("Disable Viewport Outline", "Ctrl + H", nullptr, true)) onDisableViewportOutline();
            }
            else
            {
                if (ImGui::MenuItem("Enable Viewport Outline", "Ctrl + H", nullptr, true)) onDisableViewportOutline();
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
                    ImGui::Separator();
                    const auto& componentNames = Engine::ComponentFactory::getComponentNames();
                    for (const auto& componentName : componentNames)
                        if (ImGui::MenuItem(componentName.c_str()))
                            onCreateEntity(componentName);
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
            if (m_selected.size() == 1 && m_selected.front()->hasComponent<Engine::PFXComponent>())
                m_selected.front()->getComponent<Engine::PFXComponent>()->update(dt);
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
                m_positionTarget = m_position;
            }
            else if (Engine::getInput()->getMouseWheel() > 0)
            {
                // Zoom In
                auto diff_before = (m_mouseWorldPos - m_position) * m_zoomf;
                m_zoom = std::max(0, m_zoom - 1);
                auto diff_after = (m_mouseWorldPos - m_position) * ZOOM_LEVELS[m_zoom];
                m_positionTarget = m_position + (diff_after - diff_before) / ZOOM_LEVELS[m_zoom];
            }
            else if (Engine::getInput()->getMouseWheel() < 0)
            {
                // Zoom out
                auto diff_before = (m_mouseWorldPos - m_position) * m_zoomf;
                m_zoom = std::min(7, m_zoom + 1);
                auto diff_after = (m_mouseWorldPos - m_position) * ZOOM_LEVELS[m_zoom];
                m_positionTarget = m_position + (diff_after - diff_before) / ZOOM_LEVELS[m_zoom];
            }
        }

        // Update zoom
        auto zoomTarget = ZOOM_LEVELS[m_zoom];
        m_zoomf = Engine::Utils::lerp(m_zoomf, zoomTarget, std::min(1.0f, dt * 50.0f));

        // Update position
        m_position = Engine::Utils::lerp(m_position, m_positionTarget, std::min(1.0f, dt * 50.0f));
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

static void drawSafeFrame(const std::string& name, const glm::vec2& dim, const glm::vec4& color)
{
    const float VIEWPORT_BOUNDS_SIZE = 0.025f;

    auto sb = Engine::getSpriteBatch().get();

    auto hdim = dim / 2.0f;

    sb->drawLine(glm::vec2(Engine::SPRITE_BASE_SCALE * -hdim.x, Engine::SPRITE_BASE_SCALE * -hdim.y), glm::vec2(Engine::SPRITE_BASE_SCALE * -hdim.x, Engine::SPRITE_BASE_SCALE * hdim.y + VIEWPORT_BOUNDS_SIZE/2), VIEWPORT_BOUNDS_SIZE, color);
    sb->drawLine(glm::vec2(Engine::SPRITE_BASE_SCALE * -hdim.x, Engine::SPRITE_BASE_SCALE * hdim.y), glm::vec2(Engine::SPRITE_BASE_SCALE * hdim.x + VIEWPORT_BOUNDS_SIZE/2, Engine::SPRITE_BASE_SCALE * hdim.y), VIEWPORT_BOUNDS_SIZE, color);
    sb->drawLine(glm::vec2(Engine::SPRITE_BASE_SCALE * hdim.x, Engine::SPRITE_BASE_SCALE * hdim.y), glm::vec2(Engine::SPRITE_BASE_SCALE * hdim.x, Engine::SPRITE_BASE_SCALE * -hdim.y - VIEWPORT_BOUNDS_SIZE/2), VIEWPORT_BOUNDS_SIZE, color);
    sb->drawLine(glm::vec2(Engine::SPRITE_BASE_SCALE * hdim.x, Engine::SPRITE_BASE_SCALE * -hdim.y), glm::vec2(Engine::SPRITE_BASE_SCALE * -hdim.x - VIEWPORT_BOUNDS_SIZE/2, Engine::SPRITE_BASE_SCALE * -hdim.y), VIEWPORT_BOUNDS_SIZE, color);

    auto pFont = Engine::getResourceManager()->getFont("fonts/defaultFont24.json");
    pFont->draw(name, glm::vec2(Engine::SPRITE_BASE_SCALE * -hdim.x, Engine::SPRITE_BASE_SCALE * hdim.y + VIEWPORT_BOUNDS_SIZE * 2), color, 0, 0.005f, glm::vec2(0, 0));
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

    switch (m_editDocumentType)
    {
        case EditDocumentType::Scene:
        {
            Engine::getScene()->draw();

            // Draw gizmos (Selection boxes)
            const glm::vec4 SELECTED_COLOR = { 1, 0, 0, 1 };
            const glm::vec4 HOVER_COLOR = { 1, 1, 0, 1 };

            if (!m_boxSelect)
            {
                for (const auto& pEntity : m_selected)
                    pEntity->drawOutline(SELECTED_COLOR, 1.0f / m_zoomf);

                auto pHovered = Engine::getScene()->getHoveredEntity();
                if (pHovered && !pHovered->isSelected)
                    pHovered->drawOutline(HOVER_COLOR, 1.0f / m_zoomf);
            }

            break;
        }
        case EditDocumentType::PFX:
            if (m_pPfxInstance) m_pPfxInstance->draw({0, 0});
            break;
    }

    const glm::vec4 MID_GRID_COLOR(0.5f);

    if (m_isGridVisible && m_zoomf >= m_gridHideZoomLevel)
    {
        glm::vec2 realGridOffset = glm::vec2(fmod(m_position.x, 1.0f), fmod(m_position.y, 1.0f));

        float maxVal = resolution.x > resolution.y ? resolution.x : resolution.y;

        float gridStart = maxVal / -m_zoomf - 2.0f;
        float gridEnd = maxVal / m_zoomf + 2.0f;

        for (int i = (int)gridStart; i < gridEnd; i++)
        {
            const glm::vec2 cell = glm::vec2(m_position.x + i - realGridOffset.x, m_position.y + gridStart / 2 - realGridOffset.y);

            sb->drawRect(nullptr, glm::vec4(cell.x, cell.y, 1.0f / m_zoomf, gridEnd), (cell.x == 0) ? MID_GRID_COLOR : m_gridColor);
        }

        for (int i = (int)gridStart; i < gridEnd; i++)
        {
            const glm::vec2 cell = glm::vec2(m_position.x + gridStart / 2 - realGridOffset.x, m_position.y + i - realGridOffset.y);

            sb->drawRect(nullptr, glm::vec4(cell.x, cell.y, gridEnd, 1.0f / m_zoomf), (cell.y == 0) ? MID_GRID_COLOR : m_gridColor);
        }
    }

    const glm::vec4 VIEWPORT_BOUNDS_COLOR(255, 0, 155, 155);

    if (m_isViewportOutlined && m_zoomf >= m_gridHideZoomLevel)
    {
        drawSafeFrame("2K", {1280 * 2, 720 * 2}, glm::vec4(1, 0, 0.6f, 1) * 0.8f);
        //drawSafeFrame("1080p", {1920, 1080}, glm::vec4(1, 0, 0.6f, 1) * 0.6f);
        //drawSafeFrame("720p", {1280, 720}, glm::vec4(1, 0, 0.6f, 1) * 0.4f);
    }

    if (m_boxSelect)
    {
        auto boxSelectTo = m_mouseWorldPos;
        glm::vec4 selectRect(
            glm::min(boxSelectTo.x, m_boxSelectFrom.x),
            glm::min(boxSelectTo.y, m_boxSelectFrom.y),
            glm::abs(boxSelectTo.x - m_boxSelectFrom.x),
            glm::abs(boxSelectTo.y - m_boxSelectFrom.y)
        );

        auto lineW = 1.0f / m_zoomf;
        sb->drawLine({selectRect.x, selectRect.y}, {selectRect.x, selectRect.y + selectRect.w}, lineW, {1, 1, 0, 1});
        sb->drawLine({selectRect.x, selectRect.y + selectRect.w}, {selectRect.x + selectRect.z, selectRect.y + selectRect.w}, lineW, {1, 1, 0, 1});
        sb->drawLine({selectRect.x + selectRect.z, selectRect.y + selectRect.w}, {selectRect.x + selectRect.z, selectRect.y}, lineW, {1, 1, 0, 1});
        sb->drawLine({selectRect.x + selectRect.z, selectRect.y}, {selectRect.x, selectRect.y}, lineW, {1, 1, 0, 1});
    }


    sb->end();
}

void EditorState::serializeSelectionState()
{
     //for (const auto& pEntity : m_selected)
     //{
     //    pEntity->undoJson = pEntity->serialize(false);
     //}
}

void EditorState::changeSelection(const std::vector<Engine::EntityRef>& in_newSelection)
{
    for (const auto& pEntity : m_selected)
        pEntity->isSelected = false;

    m_selected = in_newSelection;

    for (const auto& pEntity : m_selected)
    {
        pEntity->isSelected = true;
        pEntity->expand();
    }

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
    std::dynamic_pointer_cast<Game>(Engine::getGame())->changeState(std::make_shared<MainMenuState>());
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
    onCopy();
    onDelete();
}

void EditorState::onCopy()
{
    Json::Value json;
    json["version"] = Engine::FILES_VERSION;
    json["type"] = "REDDY Scene Clipboard";

    auto entities = m_selected;

    // Remove any entities that is child of already a selected one. It will deep copy anyway
    for (auto it = m_selected.begin(); it != m_selected.end(); ++it)
    {
        const auto& pParent = *it;
        for (auto it2 = entities.begin(); it2 != entities.end();)
        {
            const auto& pEntity = *it2;
            if (pParent->hasChild(pEntity, true))
            {
                it2 = entities.erase(it2);
                continue;
            }
            ++it2;
        }
    }

    Json::Value jsonEntities(Json::arrayValue);
    for (const auto& pEntity : entities)
    {
        if (!pEntity->getParent()) continue; // Ignore Root
        jsonEntities.append(pEntity->serialize(true));
    }

    Json::Value jsonParents(Json::arrayValue);
    for (const auto& pEntity : entities)
    {
        if (!pEntity->getParent()) continue; // Ignore Root
        jsonParents.append(pEntity->getParent()->id);
    }

    json["entities"] = jsonEntities;
    json["parentIds"] = jsonParents;
    
    Json::StyledWriter styledWriter;
    auto str = styledWriter.write(json);
    SDL_SetClipboardText(str.c_str());
}

void EditorState::onPaste()
{
    auto clipboard = SDL_GetClipboardText();
    if (!clipboard) return;
    
    std::stringstream ss(clipboard);
    Json::Value json;
    try
    {
        ss >> json;
    }
    catch (...)
    {
        CORE_WARN("Paste: Invalid clipboard data");
        return;
    }

    if (json["type"] != "REDDY Scene Clipboard")
    {
        CORE_WARN("Paste: Invalid clipboard data, wrong type");
        return;
    }

    const auto& entitiesJson = json["entities"];
    const auto& parentIdsJson = json["parentIds"];
    if (entitiesJson.size() != parentIdsJson.size())
    {
        CORE_WARN("Paste: Invalid clipboard data, invalid parent ids");
        return;
    }
    if (entitiesJson.empty()) return; // Nothing to paste

    decltype(m_selected) newSelection;
    for (int i = 0, len = (int)entitiesJson.size(); i < len; ++i)
    {
        const auto& entityJson = entitiesJson[i];
        auto pParent = Engine::getScene()->findEntity(parentIdsJson[i].asUInt64());
        auto pNewEntity = Engine::getScene()->createEntityFromJson(pParent, entityJson, true);
        newSelection.push_back(pNewEntity);
    }

    // Find median, then adjust entities' position to where the mouse is
    glm::vec2 bbmin = newSelection.front()->getWorldPosition();
    glm::vec2 bbmax = newSelection.front()->getWorldPosition();
    for (int i = 1, len = (int)newSelection.size(); i < len; ++i)
    {
        const auto& pEntity = newSelection[i];
        bbmin = glm::min(bbmin, pEntity->getWorldPosition());
        bbmax = glm::max(bbmax, pEntity->getWorldPosition());
    }
    auto mid = (bbmin + bbmax) * 0.5f;
    auto diff = m_mouseWorldPos - mid;
    for (const auto& pEntity : newSelection)
    {
        pEntity->setWorldPosition(pEntity->getWorldPosition() + diff);
    }

    changeSelection(newSelection);
    pushUndo("Paste");
}

void EditorState::onDuplicate()
{
    // C'est magique
    onCopy();
    onPaste();
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
    pEntity->setWorldPosition(m_mouseWorldPos);
    changeSelection({pEntity});
    pushUndo("Create Entity");
}

void EditorState::onCreateEmptyEntity()
{
    auto pEntity = Engine::getScene()->createEntity();
    createEntityAction(pEntity);
}

void EditorState::onCreateEntity(const std::string& typeName)
{
    auto pEntity = Engine::getScene()->createEntity();
    pEntity->addComponent(Engine::ComponentFactory::create(typeName));
    if (typeName == "FrameAnim")
        pEntity->addComponent(Engine::ComponentFactory::create("Sprite"));
    createEntityAction(pEntity);
}

void EditorState::onDisableGrid()
{
    switch (m_isGridVisible)
    {
        case true:
            m_isGridVisible = false;
            break;
        case false:
            m_isGridVisible = true;
            break;
    }
}

void EditorState::onDisableViewportOutline()
{
    switch (m_isViewportOutlined)
    {
        case true:
            m_isViewportOutlined = false;
            break;
        case false:
            m_isViewportOutlined = true;
            break;
    }
}

void EditorState::onDeselect()
{
    if (m_selected.empty()) return;
    changeSelectionAction({});
}

void EditorState::onSelectAll()
{
    std::vector<Engine::EntityRef> entities;
    Engine::getScene()->getRoot()->getVisibleEntities(entities);
    if (m_selected.size() == entities.size()) return;
    changeSelectionAction(entities);
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

    m_positionTarget = Engine::Utils::deserializeJsonValue<glm::vec2>(json["camera"]["position"]);
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
    m_positionTarget = m_position = {0, 0};
    m_zoom = 2;
    m_zoomf = ZOOM_LEVELS[2];
    m_prevJson = Json::Value();
    m_selectedIds.clear();
    m_selected.clear();
}
