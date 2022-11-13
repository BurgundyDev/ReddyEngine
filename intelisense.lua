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
-- Events
---------------------------------------------------------------------
-- function SendEvent(eventName, data) end
-- function RegisterEvent(eventName, c, callback) end
-- function DeregisterEvent(eventName, c) end


---------------------------------------------------------------------
-- Component-Entity functions
---------------------------------------------------------------------

-- function GetEntity(c) end
-- function GetComponent(obj, componentName) end

-- Following methods we can pass either a component or an entity.

-- Transforms
function GetPosition(obj) end
function SetPosition(obj, pos) end
function GetWorldPosition(obj) end
function SetWorldPosition(obj, pos) end
function GetRotation(obj) end
function SetRotation(obj, degrees) end
function GetScale(obj) end
function SetScale(obj, scale) end

-- Parenting
-- function GetParent(obj) end
-- function AddChild(obj) end

-- Lifecycle
-- function Enable(obj) end
-- function Disable(obj) end
-- function Destroy(obj) end -- Destroys the entity
-- function AddComponent(obj, componentName) end
-- function RemoveComponent(obj, componentName) end


---------------------------------------------------------------------
-- Entity searching
---------------------------------------------------------------------

-- function FindEntityByName(entityName, searchRadius) end -- Pass 0 for Radius for whole world
-- function FindEntityByComponent(componentTypeName, searchRadius) end -- Pass 0 for Radius for whole world
