#include "EditorState.h"
#include "ActionManager.h"
#include "Game.h"
#include "MainMenuState.h"

#include <Engine/Config.h>
#include <Engine/Log.h>
#include <Engine/PFX.h>
#include <Engine/ReddyEngine.h>
#include <Engine/Scene.h>
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

    // Layers
    if (ImGui::Begin("Layers"))
    {
        ImGui::End();
    }

    // Inspector (For selected entity/entities)
    if (ImGui::Begin("Inspector"))
    {
        ImGui::End();
    }
}


//-----------------------------------------------------------------------
// Actions from commands or shortcuts
//-----------------------------------------------------------------------


void EditorState::onNew(EditDocumentType documentType)
{
    if (!askSaveUnsavedChanges()) return;

    // Clear everything
    m_editDocumentType = documentType;
    m_dirty = false;
    m_filename = "untitled";
    m_pActionManager->clear();
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
        Engine::Scene::deserialize(json);
    }
    else if (type == "pfx")
    {
        m_pPfx = Engine::PFX::createFromFile(filename);
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

