-- This file is not loaded by the engine, only purpose is for intelisense

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


---------------------------------------------------------------------
-- Audio
---------------------------------------------------------------------
function PlaySound(filename, vol, bal, pitch) end


---------------------------------------------------------------------
-- Events
---------------------------------------------------------------------
-- function SendEvent(eventName, data) end
-- function RegisterEvent(eventName, c, callback) end
-- function DeregisterEvent(eventName, c) end


---------------------------------------------------------------------
-- Component-Entity functions
---------------------------------------------------------------------

-- Any functions that takes an entity (e) can accept either:
--   - Entity name
--   - Entity object
--   - Component object

-- function GetEntity(c) end -- Pass in component or name of entity
-- function GetComponent(obj, componentName) end

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
-- function GetParent(obj) end
-- function AddChild(obj) end

-- Lifecycle
-- function Enable(obj) end
-- function Disable(obj) end
function Destroy(e) end -- Destroys the entity (Accepts: component, entity or entity name)
-- function AddComponent(obj, componentName) end
-- function RemoveComponent(obj, componentName) end


---------------------------------------------------------------------
-- Entity searching
---------------------------------------------------------------------

-- function FindEntityByName(entityName, searchRadius) end -- Pass 0 for Radius for whole world
-- function FindEntityByComponent(componentTypeName, searchRadius) end -- Pass 0 for Radius for whole world
