#include "EditorState.h"
#include "ActionManager.h"
#include "Game.h"
#include "MainMenuState.h"

#include <Engine/Config.h>
#include <Engine/ReddyEngine.h>

#include <imgui.h>


EditorState::EditorState()
{
    m_pActionManager = std::make_shared<ActionManager>();
}

void EditorState::update(float dt)
{
    // Menu bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) onNew();
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

void EditorState::onNew()
{
}

void EditorState::onOpen()
{
}

void EditorState::onSave()
{
}

void EditorState::onSaveAs()
{
}

void EditorState::onQuit()
{
    // Go back to main menu
    g_pGame->changeState(std::make_shared<MainMenuState>());
}

void EditorState::open(const std::string& filename)
{
}

void EditorState::save(const std::string& filename)
{
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
