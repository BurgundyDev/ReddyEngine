#include "EditorState.h"
#include "ActionManager.h"

#include <Engine/Input.h>
#include <Engine/PFX.h>
#include <Engine/ReddyEngine.h>
#include <Engine/ResourceManager.h>
#include <Engine/Texture.h>
#include <Engine/GUI.h>


void EditorState::drawPFXUI()
{
    if (Engine::GUI::beginEditorWindow("PFX Inspector"))
    {
        bool changed = false;
        int i = 0;
        for (auto it = m_pPfx->emitters.begin(); it != m_pPfx->emitters.end(); )
        {
            auto& emitter = *it;

            ++i;
            Engine::GUI::SectionState nav;
            if ((nav = Engine::GUI::beginSection(emitter.name)) == Engine::GUI::SectionState::Open)
            {
                changed |= Engine::GUI::stringProperty("Name", &emitter.name);
                changed |= Engine::GUI::enumProperty("Type", &emitter.type, "burst\0continuous");
                
                switch (emitter.type)
                {
                    case Engine::EmitterType::burst:
                        changed |= Engine::GUI::floatProperty("Burst Duration", &emitter.burstDuration, 0.0f, 60.0f * 20.0f);
                        changed |= Engine::GUI::intProperty("Burst Amount", &emitter.burstAmount, 1, 1000);
                        break;
                    case Engine::EmitterType::continuous:
                        changed |= Engine::GUI::floatProperty("Spawn Rate", &emitter.spawnRate, 0.0f);
                        break;
                }

                changed |= Engine::GUI::textureProperty("Texture", &emitter.pTexture);
                changed |= Engine::GUI::floatSliderProperty("Spread", &emitter.spread, 0.0f, 360.0f);


                // Color
                Engine::GUI::beginGroup("Color");
                {
                    changed |= Engine::GUI::boolProperty("Same Start-End", &emitter.color.sameStartEnd);
                    changed |= Engine::GUI::boolProperty("Random Start", &emitter.color.randomStart);
                    changed |= Engine::GUI::colorProperty("Start Color 1", &emitter.color.startRange[0]);
                    if (emitter.color.randomStart)
                    {
                        changed |= Engine::GUI::colorProperty("Start Color 2", &emitter.color.startRange[1]);
                    }
                    if (!emitter.color.sameStartEnd)
                    {
                        changed |= Engine::GUI::boolProperty("Random End", &emitter.color.randomEnd);
                        changed |= Engine::GUI::colorProperty("End Color 1", &emitter.color.endRange[0]);
                        if (emitter.color.randomEnd)
                        {
                            changed |= Engine::GUI::colorProperty("End Color 2", &emitter.color.endRange[1]);
                        }
                        changed |= Engine::GUI::boolProperty("End only affect Alpha", &emitter.endOnlyAffectAlpha);
                    }
                }
                Engine::GUI::endGroup();

                // additive
                Engine::GUI::beginGroup("Additive");
                {
                    changed |= Engine::GUI::boolProperty("Same Start-End", &emitter.additive.sameStartEnd);
                    changed |= Engine::GUI::boolProperty("Random Start", &emitter.additive.randomStart);
                    changed |= Engine::GUI::floatSliderProperty("Start Additive 1", &emitter.additive.startRange[0], 0.0f, 1.0f);
                    if (emitter.additive.randomStart)
                    {
                        changed |= Engine::GUI::floatSliderProperty("Start Additive 2", &emitter.additive.startRange[1], 0.0f, 1.0f);
                    }
                    if (!emitter.additive.sameStartEnd)
                    {
                        changed |= Engine::GUI::boolProperty("Random End", &emitter.additive.randomEnd);
                        changed |= Engine::GUI::floatSliderProperty("End Additive 1", &emitter.additive.endRange[0], 0.0f, 1.0f);
                        if (emitter.additive.randomEnd)
                        {
                            changed |= Engine::GUI::floatSliderProperty("End Additive 2", &emitter.additive.endRange[1], 0.0f, 1.0f);
                        }
                    }
                }
                Engine::GUI::endGroup();

                // size
                Engine::GUI::beginGroup("Size");
                {
                    changed |= Engine::GUI::boolProperty("Same Start-End", &emitter.size.sameStartEnd);
                    changed |= Engine::GUI::boolProperty("Random Start", &emitter.size.randomStart);
                    changed |= Engine::GUI::floatProperty("Start Size 1", &emitter.size.startRange[0]);
                    if (emitter.size.randomStart)
                    {
                        changed |= Engine::GUI::floatProperty("Start Size 2", &emitter.size.startRange[1]);
                    }
                    if (!emitter.size.sameStartEnd)
                    {
                        changed |= Engine::GUI::boolProperty("Random End", &emitter.size.randomEnd);
                        changed |= Engine::GUI::floatProperty("End Size 1", &emitter.size.endRange[0]);
                        if (emitter.size.randomEnd)
                        {
                            changed |= Engine::GUI::floatProperty("End Size 2", &emitter.size.endRange[1]);
                        }
                    }
                }
                Engine::GUI::endGroup();

                // Rotation
                Engine::GUI::beginGroup("Rotation");
                {
                    changed |= Engine::GUI::boolProperty("Random", &emitter.rotation.random);
                    changed |= Engine::GUI::floatProperty("Rot 1", &emitter.rotation.range[0]);
                    if (emitter.rotation.random)
                    {
                        changed |= Engine::GUI::floatProperty("Rot 2", &emitter.rotation.range[1]);
                    }
                }
                Engine::GUI::endGroup();

                // Rotation Speed
                Engine::GUI::beginGroup("Rotation Speed");
                {
                    changed |= Engine::GUI::boolProperty("Same Start-End", &emitter.rotationSpeed.sameStartEnd);
                    changed |= Engine::GUI::boolProperty("Random Start", &emitter.rotationSpeed.randomStart);
                    changed |= Engine::GUI::floatProperty("Start Rot Speed 1", &emitter.rotationSpeed.startRange[0]);
                    if (emitter.rotationSpeed.randomStart)
                    {
                        changed |= Engine::GUI::floatProperty("Start Rot Speed 2", &emitter.rotationSpeed.startRange[1]);
                    }
                    if (!emitter.rotationSpeed.sameStartEnd)
                    {
                        changed |= Engine::GUI::boolProperty("Random End", &emitter.rotationSpeed.randomEnd);
                        changed |= Engine::GUI::floatProperty("End Rot Speed 1", &emitter.rotationSpeed.endRange[0]);
                        if (emitter.rotationSpeed.randomEnd)
                        {
                            changed |= Engine::GUI::floatProperty("End Rot Speed 2", &emitter.rotationSpeed.endRange[1]);
                        }
                    }
                }
                Engine::GUI::endGroup();

                // Speed
                Engine::GUI::beginGroup("Speed");
                {
                    changed |= Engine::GUI::boolProperty("Same Start-End", &emitter.speed.sameStartEnd);
                    changed |= Engine::GUI::boolProperty("Random Start", &emitter.speed.randomStart);
                    changed |= Engine::GUI::floatProperty("Start Speed 1", &emitter.speed.startRange[0]);
                    if (emitter.speed.randomStart)
                    {
                        changed |= Engine::GUI::floatProperty("Start Speed 2", &emitter.speed.startRange[1]);
                    }
                    if (!emitter.speed.sameStartEnd)
                    {
                        changed |= Engine::GUI::boolProperty("Random End", &emitter.speed.randomEnd);
                        changed |= Engine::GUI::floatProperty("End Speed 1", &emitter.speed.endRange[0]);
                        if (emitter.speed.randomEnd)
                        {
                            changed |= Engine::GUI::floatProperty("End Speed 2", &emitter.speed.endRange[1]);
                        }
                    }
                }
                Engine::GUI::endGroup();

                // Duration
                Engine::GUI::beginGroup("Duration");
                {
                    changed |= Engine::GUI::boolProperty("Random", &emitter.duration.random);
                    changed |= Engine::GUI::floatProperty("Duration 1", &emitter.duration.range[0]);
                    if (emitter.duration.random)
                    {
                        changed |= Engine::GUI::floatProperty("Duration 2", &emitter.duration.range[1]);
                    }
                }
                Engine::GUI::endGroup();

                // Gravity
                Engine::GUI::beginGroup("Gravity");
                {
                    changed |= Engine::GUI::boolProperty("Same Start-End", &emitter.gravity.sameStartEnd);
                    changed |= Engine::GUI::boolProperty("Random Start", &emitter.gravity.randomStart);
                    changed |= Engine::GUI::vec2Property("Start Grav 1", &emitter.gravity.startRange[0]);
                    if (emitter.gravity.randomStart)
                    {
                        changed |= Engine::GUI::vec2Property("Start Grav 2", &emitter.gravity.startRange[1]);
                    }
                    if (!emitter.gravity.sameStartEnd)
                    {
                        changed |= Engine::GUI::boolProperty("Random End", &emitter.gravity.randomEnd);
                        changed |= Engine::GUI::vec2Property("End Grav 1", &emitter.gravity.endRange[0]);
                        if (emitter.gravity.randomEnd)
                        {
                            changed |= Engine::GUI::vec2Property("End Grav 2", &emitter.gravity.endRange[1]);
                        }
                    }
                }
                Engine::GUI::endGroup();

                // Spawn Radius
                Engine::GUI::beginGroup("Spawn Radius");
                {
                    changed |= Engine::GUI::boolProperty("Random", &emitter.spawnRadius.random);
                    changed |= Engine::GUI::floatProperty("Radius 1", &emitter.spawnRadius.range[0]);
                    if (emitter.spawnRadius.random)
                    {
                        changed |= Engine::GUI::floatProperty("Radius 2", &emitter.spawnRadius.range[1]);
                    }
                }
                Engine::GUI::endGroup();

                Engine::GUI::endSection();
            }

            if (nav == Engine::GUI::SectionState::Delete)
            {
                changed = true;
                it = m_pPfx->emitters.erase(it);
                continue;
            }

            if (nav == Engine::GUI::SectionState::MoveUp)
            {
                if (i > 1)
                {
                    changed = true;
                    auto copy = emitter;
                    it = m_pPfx->emitters.erase(it);
                    it = m_pPfx->emitters.insert(it - 1, copy) + 1;
                    continue;
                }
            }

            if (nav == Engine::GUI::SectionState::MoveDown)
            {
                if (i < (int)m_pPfx->emitters.size())
                {
                    changed = true;
                    auto copy = emitter;
                    it = m_pPfx->emitters.erase(it);
                    it = m_pPfx->emitters.insert(it + 1, copy) + 1;
                    continue;
                }
            }

            ++it;
        }

        if (Engine::GUI::statusBarButton("+", "Create Emitter"))
        {
            Engine::EmitterDef emitter;
            emitter.pTexture = Engine::getResourceManager()->getTexture("textures/particle.png");
            m_pPfx->emitters.push_back(emitter);
            changed = true;
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

    Engine::GUI::endEditorWindow();
}
