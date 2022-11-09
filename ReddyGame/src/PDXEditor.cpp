#include "EditorState.h"
#include "ActionManager.h"

#include <Engine/PFX.h>
#include <Engine/ReddyEngine.h>
#include <Engine/ResourceManager.h>
#include <Engine/Texture.h>

#include <imgui.h>


void EditorState::drawPFXUI()
{
    if (ImGui::Begin("PFX Inspector"))
    {
        if (ImGui::Button("Play"))
        {
            m_pPfxInstance = std::make_shared<Engine::PFXInstance>(m_pPfx);
        }
        bool changed = false;
        if (ImGui::Button("Add Emitter"))
        {
            Engine::EmitterDef emitter;
            emitter.pTexture = Engine::getResourceManager()->getTexture("textures/particle.png");
            m_pPfx->emitters.push_back(emitter);
            changed = true;
        }
        int i = 0;
        for (auto it = m_pPfx->emitters.begin(); it != m_pPfx->emitters.end(); )
        {
            auto& emitter = *it;

            ++i;
            if (ImGui::CollapsingHeader(("Emitter " + std::to_string(i)).c_str()))
            {
                ImGui::Indent(10.0f);
                
                if (ImGui::Button(("Remove Emitter " + std::to_string(i)).c_str()))
                {
                    changed = true;
                    it = m_pPfx->emitters.erase(it);
                    continue;
                }

                {
                    const char* EMITTER_TYPE_CHOICES[] = { "burst", "continuous" };
                    int current_choice = (int)emitter.type;
                    if (ImGui::Combo("Type", &current_choice, "burst\0continuous"))
                    {
                        changed |= (emitter.type != (Engine::EmitterType)current_choice);
                        emitter.type = (Engine::EmitterType)current_choice;
                    }
                }

                switch (emitter.type)
                {
                    case Engine::EmitterType::burst:
                        ImGui::DragFloat("Burst Duration", &emitter.burstDuration, 0.01f, 0.0f, 60.0f * 20.0f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        ImGui::DragInt("Burst Amount", &emitter.burstAmount, 1.0f, 1, 1000);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        break;
                    case Engine::EmitterType::continuous:
                        ImGui::DragFloat("Spawn Rate", &emitter.spawnRate);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        break;
                }

                {
                    char buf[260];
                    if (emitter.pTexture)
                        memcpy(buf, emitter.pTexture->getFilename().c_str(), emitter.pTexture->getFilename().size() + 1);
                    else
                        buf[0] = '\0';
                    ImGui::InputText("Texture", buf, 260);
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        emitter.pTexture = Engine::getResourceManager()->getTexture(buf);
                        changed = true;
                    }
                }

                ImGui::SliderFloat("Spread", &emitter.spread, 0.0f, 360.0f);

                // Color
                {
                    ImGui::Separator();
                    ImGui::Text("Color");
                    ImGui::Indent(10.0f);

                    if (ImGui::Checkbox("Same Start-End##colorsamestartend", &emitter.color.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##colorrandomstart", &emitter.color.randomStart)) changed = true;
                    ImGui::ColorEdit4("Start Color 1", &emitter.color.startRange[0].x);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.color.randomStart)
                    {
                        ImGui::ColorEdit4("Start Color 2", &emitter.color.startRange[1].x);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.color.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##colorrandomend", &emitter.color.randomEnd)) changed = true;
                        ImGui::ColorEdit4("End Color 1", &emitter.color.endRange[0].x);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.color.randomEnd)
                        {
                            ImGui::ColorEdit4("End Color 2", &emitter.color.endRange[1].x);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                        ImGui::Checkbox("End only affect Alpha", &emitter.endOnlyAffectAlpha);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }

                    ImGui::Unindent(10.0f);
                }

                // additive
                {
                    ImGui::Separator();
                    ImGui::Text("Additive");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##additivesamestartend", &emitter.additive.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##additiverandomstart", &emitter.additive.randomStart)) changed = true;
                    ImGui::SliderFloat("Start Additive 1", &emitter.additive.startRange[0], 0.0f, 1.0f);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.additive.randomStart)
                    {
                        ImGui::SliderFloat("Start Additive 2", &emitter.additive.startRange[1], 0.0f, 1.0f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.additive.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##additiverandomend", &emitter.additive.randomEnd)) changed = true;
                        ImGui::SliderFloat("End Additive 1", &emitter.additive.endRange[0], 0.0f, 1.0f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.additive.randomEnd)
                        {
                            ImGui::SliderFloat("End Additive 2", &emitter.additive.endRange[1], 0.0f, 1.0f);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // size
                {
                    ImGui::Separator();
                    ImGui::Text("Size");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##sizesamestartend", &emitter.size.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##sizerandomstart", &emitter.size.randomStart)) changed = true;
                    ImGui::DragFloat("Start Size 1", &emitter.size.startRange[0], 0.1f);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.size.randomStart)
                    {
                        ImGui::DragFloat("Start Size 2", &emitter.size.startRange[1], 0.1f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.size.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##sizerandomend", &emitter.size.randomEnd)) changed = true;
                        ImGui::DragFloat("End Size 1", &emitter.size.endRange[0], 0.1f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.size.randomEnd)
                        {
                            ImGui::DragFloat("End Size 2", &emitter.size.endRange[1], 0.1f);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // Rotation
                {
                    ImGui::Separator();
                    ImGui::Text("Rotation");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Random##rotrandom", &emitter.rotation.random)) changed = true;
                    ImGui::DragFloat("Rot 1", &emitter.rotation.range[0]);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.rotation.random)
                    {
                        ImGui::DragFloat("Rot 2", &emitter.rotation.range[1]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }

                    ImGui::Unindent(10.0f);
                }

                // Rotation Speed
                {
                    ImGui::Separator();
                    ImGui::Text("Rotation Speed");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##rotationspeedsamestartend", &emitter.rotationSpeed.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##rotspeedrandomstart", &emitter.rotationSpeed.randomStart)) changed = true;
                    ImGui::DragFloat("Start Rot Speed 1", &emitter.rotationSpeed.startRange[0], 0.1f);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.rotationSpeed.randomStart)
                    {
                        ImGui::DragFloat("Start Rot Speed 2", &emitter.rotationSpeed.startRange[1], 0.1f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.rotationSpeed.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##rotspeedrandomend", &emitter.rotationSpeed.randomEnd)) changed = true;
                        ImGui::DragFloat("End Rot Speed 1", &emitter.rotationSpeed.endRange[0], 0.1f);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.rotationSpeed.randomEnd)
                        {
                            ImGui::DragFloat("End Rot Speed 2", &emitter.rotationSpeed.endRange[1], 0.1f);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // Speed
                {
                    ImGui::Separator();
                    ImGui::Text("Speed");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##speedsamestartend", &emitter.speed.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##speedrandomstart", &emitter.speed.randomStart)) changed = true;
                    ImGui::DragFloat("Start Speed 1", &emitter.speed.startRange[0]);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.speed.randomStart)
                    {
                        ImGui::DragFloat("Start Speed 2", &emitter.speed.startRange[1]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.speed.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##speedrandomend", &emitter.speed.randomEnd)) changed = true;
                        ImGui::DragFloat("End Speed 1", &emitter.speed.endRange[0]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.speed.randomEnd)
                        {
                            ImGui::DragFloat("End Speed 2", &emitter.speed.endRange[1]);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // Duration
                {
                    ImGui::Separator();
                    ImGui::Text("Duration");
                    ImGui::Indent(10.0f);

                    if (ImGui::Checkbox("Random##durationrandom", &emitter.duration.random)) changed = true;
                    ImGui::DragFloat("Duration 1", &emitter.duration.range[0]);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.duration.random)
                    {
                        ImGui::DragFloat("Duration 2", &emitter.duration.range[1]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }

                    ImGui::Unindent(10.0f);
                }

                // Gravity
                {
                    ImGui::Separator();
                    ImGui::Text("Gravity");
                    ImGui::Indent(10.0f);
                    
                    if (ImGui::Checkbox("Same Start-End##gravitysamestartend", &emitter.gravity.sameStartEnd)) changed = true;
                    if (ImGui::Checkbox("Random Start##gravityrandomstart", &emitter.gravity.randomStart)) changed = true;
                    ImGui::DragFloat2("Start Grav 1", &emitter.gravity.startRange[0].x);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.gravity.randomStart)
                    {
                        ImGui::DragFloat2("Start Grav 2", &emitter.gravity.startRange[1].x);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }
                    if (!emitter.gravity.sameStartEnd)
                    {
                        if (ImGui::Checkbox("Random End##gravityrandomend", &emitter.gravity.randomEnd)) changed = true;
                        ImGui::DragFloat2("End Grav 1", &emitter.gravity.endRange[0].x);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        if (emitter.gravity.randomEnd)
                        {
                            ImGui::DragFloat2("End Grav 2", &emitter.gravity.endRange[1].x);
                            if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                        }
                    }

                    ImGui::Unindent(10.0f);
                }

                // Spawn Radius
                {
                    ImGui::Separator();
                    ImGui::Text("Spawn Radius");
                    ImGui::Indent(10.0f);

                    if (ImGui::Checkbox("Random##spawnradiusrandom", &emitter.spawnRadius.random)) changed = true;
                    ImGui::DragFloat("Radius 1", &emitter.spawnRadius.range[0]);
                    if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    if (emitter.spawnRadius.random)
                    {
                        ImGui::DragFloat("Radius 2", &emitter.spawnRadius.range[1]);
                        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
                    }

                    ImGui::Unindent(10.0f);
                }

                ImGui::Unindent(10.0f);
            }

            ++it;
        }

        if (changed)
        {
            setDirty(true);
            auto prevJson = m_pfxJson;
            m_pfxJson = m_pPfx->serialize();
            auto newJson = m_pfxJson;
            m_pActionManager->addAction("Edit PFX", 
                                        [this, newJson]() // Redo
            {
                m_pPfxInstance.reset();
                m_pPfx->deserialize(newJson);
                m_pfxJson = newJson;
                setDirty(true);
            },
                                        [this, prevJson]() // Undo
            {
                m_pPfxInstance.reset();
                m_pPfx->deserialize(prevJson);
                m_pfxJson = prevJson;
                setDirty(true);
            });
        }
    }
    ImGui::End();
}
