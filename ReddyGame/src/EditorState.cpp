#include "EditorState.h"
#include "ActionManager.h"
#include "Game.h"
#include "MainMenuState.h"

#include <Engine/Config.h>
#include <Engine/Log.h>
#include <Engine/PFX.h>
#include <Engine/ReddyEngine.h>
#include <Engine/ResourceManager.h>
#include <Engine/Scene.h>
#include <Engine/SpriteBatch.h>
#include <Engine/Utils.h>

#include <imgui.h>
#include <tinyfiledialogs/tinyfiledialogs.h>


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
}

void EditorState::update(float dt)
{
    // Menu bar
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
            if (ImGui::MenuItem("Quit", "Alt+F4")) onQuit();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) onUndo();
            if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z")) onRedo();
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) onCut();
            if (ImGui::MenuItem("Copy", "Ctrl+C")) onCopy();
            if (ImGui::MenuItem("Paste", "Ctrl+V")) onPaste();
            if (ImGui::MenuItem("Duplicate", "Ctrl+D")) onDuplicate();
            if (ImGui::MenuItem("Delete", "Del")) onDelete();
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }

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
}

void EditorState::drawSceneUI()
{
    // Layers
    if (ImGui::Begin("Layers"))
    {
    }
    ImGui::End();

    // Inspector (For selected entity/entities)
    if (ImGui::Begin("Entity Inspector"))
    {
    }
    ImGui::End();
}

void EditorState::drawPFXUI()
{
    if (ImGui::Begin("PFX Inspector"))
    {
        if (ImGui::Button("Play"))
        {
            m_pPfxInstance = std::make_shared<Engine::PFXInstance>(m_pPfx);
        }
        if (ImGui::Button("Add Emitter"))
        {
            Engine::EmitterDef emitter;
            emitter.pTexture = Engine::getResourceManager()->getTexture("textures/particle.png");
            m_pPfx->emitters.push_back(emitter);
        }
        int i = 0;
        for (auto& emitter : m_pPfx->emitters)
        {
            ++i;
            if (ImGui::CollapsingHeader(("Emitter " + std::to_string(i)).c_str()))
            {
                ImGui::Indent(10.0f);

                {
                    const char* EMITTER_TYPE_CHOICES[] = { "burst", "continuous" };
                    int current_choice = (int)emitter.type;
                    if (ImGui::Combo("Type", &current_choice, "burst\0continuous"))
                    {
                        emitter.type = (Engine::EmitterType)current_choice;
                    }
                }

                switch (emitter.type)
                {
                    case Engine::EmitterType::burst:
                        ImGui::DragFloat("Burst Duration", &emitter.burstDuration, 0.01f, 0.0f, 60.0f * 20.0f);
                        ImGui::DragInt("Burst Amount", &emitter.burstAmount, 1.0f, 1, 1000);
                        break;
                    case Engine::EmitterType::continuous:
                        ImGui::DragFloat("Spawn Rate", &emitter.spawnRate);
                        break;
                }

                {
                    char buf[260];
                    if (emitter.pTexture)
                        memcpy(buf, emitter.pTexture->getFilename().c_str(), emitter.pTexture->getFilename().size() + 1);
                    else
                        buf[0] = '\0';
                    ImGui::InputText("Texture", buf, 260);
                    emitter.pTexture = Engine::getResourceManager()->getTexture(buf);
                }

                ImGui::SliderFloat("Spread", &emitter.spread, 0.0f, 360.0f);

                // Color
                {
                    ImGui::Text("Color");
                    ImGui::Indent(10.0f);

                    ImGui::Checkbox("Random Start##colorrandomstart", &emitter.color.randomStart);
                    ImGui::ColorEdit4("Start Color 1", &emitter.color.startRange[0].x);
                    if (emitter.color.randomStart)
                        ImGui::ColorEdit4("Start Color 2", &emitter.color.startRange[1].x);
                    ImGui::Checkbox("Random End##colorrandomend", &emitter.color.randomEnd);
                    ImGui::ColorEdit4("End Color 1", &emitter.color.endRange[0].x);
                    if (emitter.color.randomEnd)
                        ImGui::ColorEdit4("End Color 2", &emitter.color.endRange[1].x);
                    ImGui::Checkbox("End only affect Alpha", &emitter.endOnlyAffectAlpha);

                    ImGui::Unindent(10.0f);
                }

                // additive
                {
                    ImGui::Text("Additive");
                    ImGui::Indent(10.0f);

                    ImGui::Checkbox("Random Start##additiverandomstart", &emitter.additive.randomStart);
                    ImGui::SliderFloat("Start Additive 1", &emitter.additive.startRange[0], 0.0f, 1.0f);
                    if (emitter.additive.randomStart)
                        ImGui::SliderFloat("Start Additive 2", &emitter.additive.startRange[1], 0.0f, 1.0f);
                    ImGui::Checkbox("Random End##additiverandomend", &emitter.additive.randomEnd);
                    ImGui::SliderFloat("End Additive 1", &emitter.additive.endRange[0], 0.0f, 1.0f);
                    if (emitter.additive.randomEnd)
                        ImGui::SliderFloat("End Additive 2", &emitter.additive.endRange[1], 0.0f, 1.0f);

                    ImGui::Unindent(10.0f);
                }

                // size
                {
                    ImGui::Text("Additive");
                    ImGui::Indent(10.0f);

                    ImGui::Checkbox("Random Start##sizerandomstart", &emitter.size.randomStart);
                    ImGui::DragFloat("Start Size 1", &emitter.size.startRange[0], 0.1f);
                    if (emitter.size.randomStart)
                        ImGui::DragFloat("Start Size 2", &emitter.size.startRange[1], 0.1f);
                    ImGui::Checkbox("Random End##sizerandomend", &emitter.size.randomEnd);
                    ImGui::DragFloat("End Size 1", &emitter.size.endRange[0], 0.1f);
                    if (emitter.size.randomEnd)
                        ImGui::DragFloat("End Size 2", &emitter.size.endRange[1], 0.1f);

                    ImGui::Unindent(10.0f);
                }

                // size
                {
                    ImGui::Text("Speed");
                    ImGui::Indent(10.0f);

                    ImGui::Checkbox("Random Start##speedrandomstart", &emitter.speed.randomStart);
                    ImGui::DragFloat("Start Speed 1", &emitter.speed.startRange[0]);
                    if (emitter.speed.randomStart)
                        ImGui::DragFloat("Start Speed 2", &emitter.speed.startRange[1]);
                    ImGui::Checkbox("Random End##speedrandomend", &emitter.speed.randomEnd);
                    ImGui::DragFloat("End Speed 1", &emitter.speed.endRange[0]);
                    if (emitter.speed.randomEnd)
                        ImGui::DragFloat("End Speed 2", &emitter.speed.endRange[1]);

                    ImGui::Unindent(10.0f);
                }

                // Duration
                {
                    ImGui::Text("Duration");
                    ImGui::Indent(10.0f);

                    ImGui::Checkbox("Random##durationrandom", &emitter.duration.random);
                    ImGui::DragFloat("Duration 1", &emitter.duration.range[0]);
                    if (emitter.duration.random)
                        ImGui::DragFloat("Duration 2", &emitter.duration.range[1]);

                    ImGui::Unindent(10.0f);
                }

                ImGui::Unindent(10.0f);
            }
        }
    }
    ImGui::End();
}

void EditorState::draw()
{
    auto sb = Engine::getSpriteBatch().get();
    sb->begin();

    switch (m_editDocumentType)
    {
        case EditDocumentType::Scene:
            break;
        case EditDocumentType::PFX:
        {
            // Draw faint cross in the middle so we know where's the center
            sb->drawRect(nullptr, glm::vec4(
                Engine::getResolution().x * 0.5f + (-m_position.x - 1.0f) * m_zoomf,
                Engine::getResolution().y * 0.5f + (-m_position.y) * m_zoomf,
                2.0f * m_zoomf, 1.0f),
                glm::vec4(0.35f));
            sb->drawRect(nullptr, glm::vec4(
                Engine::getResolution().x * 0.5f + (-m_position.x) * m_zoomf,
                Engine::getResolution().y * 0.5f + (-m_position.y - 1.0f) * m_zoomf,
                1.0f, 2.0f * m_zoomf),
                glm::vec4(0.35f));
            if (m_pPfxInstance) m_pPfxInstance->draw(Engine::getResolution() * 0.5f - m_position * m_zoomf, 0.0f, m_zoomf);
            break;
        }
    }

    sb->end();
}


//-----------------------------------------------------------------------
// Actions from commands or shortcuts
//-----------------------------------------------------------------------


void EditorState::onNew(EditDocumentType documentType)
{
    if (!askSaveUnsavedChanges()) return;

    // Clear everything
    m_editDocumentType = documentType;
    m_dirty = true;
    m_filename = "untitled";
    m_pActionManager->clear();

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
    if (!m_dirty) return; // Don't waste processing to save if nothing is changed
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
}

void EditorState::onRedo()
{
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


//-----------------------------------------------------------------------
// File dialog bullshit
//-----------------------------------------------------------------------


void EditorState::open(const std::string& filename)
{
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
        Engine::Scene::deserialize(json);
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
    
    m_filename = filename;
    m_pActionManager->clear();
    m_dirty = false;
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
    m_dirty = false;
    addRecentFile(m_filename);
}

bool EditorState::saveAs()
{
    auto filename = tinyfd_saveFileDialog("Save", "./assets/scenes/", 1, FILE_PATTERNS, "json files");
    if (!filename) return false;

    m_filename = filename;
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

