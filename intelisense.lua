-- This file is not loaded by the engine, only purpose is for intelisense

-- Any functions that takes an entity (e) can accept either:
--   - Entity name
--   - Entity object
--   - Component object

---------------------------------------------------------------------
-- Utilities
---------------------------------------------------------------------
function Log(text) end
function GetScreenRect() end -- Returns 4 values: x,y,w,h
function EmitParticles(pfx, position, angle, scale) end -- Returns the newly created entity containing the PFXComponent


---------------------------------------------------------------------
-- Defining a component
---------------------------------------------------------------------
function RegisterComponent(name, c) end
function SetIntProperty(name, tooltip) end
function SetFloatProperty(name, tooltip) end
function SetVec2Property(name, tooltip) end
function SetColorProperty(name, tooltip) end
function SetStringProperty(name, tooltip) end


---------------------------------------------------------------------
-- Math functions
---------------------------------------------------------------------
function Length(v) end
function Distance(a, b) end
function Normalize(v) end
function Dot(a, b) end


---------------------------------------------------------------------
-- Camera functions
---------------------------------------------------------------------
function GetCameraPosition() end
function SetCameraPosition(pos) end
function GetCameraZoom() end
function SetCameraZoom(zoom) end


---------------------------------------------------------------------
-- Inputs
---------------------------------------------------------------------
function IsKeyDown(key) end
function IsButtonDown(button) end
function IsButtonJustDown(button) end
function GetMousePosition() end


---------------------------------------------------------------------
-- Audio
---------------------------------------------------------------------
function PlaySound(filename, vol, bal, pitch) end
function PlayMusic(filename) end
function StopMusic() end
function PauseMusic() end
function ResumeMusic() end


---------------------------------------------------------------------
-- Events
---------------------------------------------------------------------
function SendEvent(eventName, data) end
function RegisterEvent(eventName, c, functionName) end
function DeregisterEvent(eventName, c) end


---------------------------------------------------------------------
-- Entity functions
---------------------------------------------------------------------
function GetEntity(c) end -- Pass in component or name of entity (entity also works, but pointless)
function GetRoot() end
function GetComponent(e, componentName) end -- Returns the LUA component. You cannot get Built-in components.
function GetName(e) end
function SetName(e, name) end

-- Transforms
function GetPosition(e) end
function SetPosition(e, pos) end
function GetWorldPosition(e) end
function SetWorldPosition(e, pos) end
function GetRotation(e) end
function SetRotation(e, degrees) end
function GetScale(e) end
function SetScale(e, scale) end

-- Parenting
function GetParent(e) end
function AddChild(parent, e) end

-- Lifecycle
function EnableEntity(e) end
function DisableEntity(e) end
function EnableComponent(e, componentName) end -- Lua name, or built-ins: Sprite, Text, etc.
function DisableComponent(e, componentName) end
function CreateEntity(parent) end
function CreateEntity(parent, prefabName) end
function Destroy(e) end -- Destroys the entity (Accepts: component, entity or entity name)
function AddComponent(e, componentName) end -- Lua name, or built-ins: Sprite, Text, etc.
function RemoveComponent(e, componentName) end


---------------------------------------------------------------------
-- Components functions
---------------------------------------------------------------------
function GetSpriteTexture(e) end
function SetSpriteTexture(e, filename) end
function GetSpriteColor(e) end
function SetSpriteColor(e, color) end
function GetSpriteOrigin(e) end
function SetSpriteOrigin(e, origin) end

function GetFont(e) end
function SetFont(e, filename) end
function GetText(e) end
function SetText(e, text) end
function GetTextColor(e) end
function SetTextColor(e, color) end
function GetTextOrigin(e) end
function SetTextOrigin(e, origin) end
function GetTextScale(e) end
function SetTextScale(e, scale) end

function GetPFX(e) end
function SetPFX(e, filename) end
function PlayPFX(e) end
function StopPFX(e) end

function PlayFrameAnim(e, animAnme) end


---------------------------------------------------------------------
-- Entity searching
---------------------------------------------------------------------
function FindEntityByName(entityName, pos, searchRadius) end -- Pass 0 for Radius for whole world
function FindEntitiesByName(entityName, pos, searchRadius) end -- Returns array. Pass 0 for Radius for whole world.
function FindEntityByComponent(componentTypeName, pos, searchRadius) end -- Pass 0 for Radius for whole world
function FindEntitiesByComponent(componentTypeName, pos, searchRadius) end -- eturns array. Pass 0 for Radius for whole world.


---------------------------------------------------------------------
-- State Management
---------------------------------------------------------------------
function ContinueGame() end -- Continue previously started game
function NewGame(filename) end -- Start a new game
function Quit() end -- Close the program
function Editor() end -- Go to the Editor state
function MainMenu() end -- Quit game back to main menu
function Resume() end -- Resume game if we're in the ingame menu
