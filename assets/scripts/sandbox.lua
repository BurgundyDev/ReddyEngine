-- Particle that moves to mouse position after pressing LMB
RegisterComponent("MovingParticle", {
    mouseActivationDistance = 0.4,
    targetDistance = 0.1,
    speed = 0.05,
    targetPosition = Vec2(0, 0),

    initComponent = function()
        SetFloatProperty("mouseActivationDistance", "Mouse distance from Player to activate movement.")
        SetFloatProperty("targetDistance",
            "Player target distance to stop. Player also stops after reaching this distance value.")
        SetFloatProperty("speed", "Speed of Player movement.")
    end,

    create = function(self)
        self.targetPosition = GetWorldPosition(self)
    end,

    fixedUpdate = function(self)
        local distanceToTarget = Distance(GetWorldPosition(self), self.targetPosition)
        local isFarFromTarget = distanceToTarget > self.targetDistance
        if isFarFromTarget then
            SetWorldPosition(self,
                GetWorldPosition(self) + Normalize(self.targetPosition - GetWorldPosition(self)) * self.speed)
        end
    end,

    update = function(self)
        local isMousePressed = IsButtonJustDown(MOUSE_BUTTON_LEFT)
        local isCursorFar = Distance(GetMouseWorldPosition(), GetWorldPosition(self)) > self.mouseActivationDistance

        if isCursorFar and isMousePressed then
            self.targetPosition = GetMouseWorldPosition()
        end
    end
})

-- Rotates and scale
RegisterComponent("sandboxRotator", {
    rotSpeed = 0,
    scaleSpeed = 0,
    scale = 0,

    initComponent = function()
        SetFloatProperty("rotSpeed", "Rotate speed")
        SetFloatProperty("scaleSpeed", "Scale speed")
    end,

    fixedUpdate = function(self, dt)
        self.scale = self.scale + self.scaleSpeed * dt;
        SetScale(self, Vec2(math.sin(math.rad(self.scale)) * 0.25 + 1))
    end,

    update = function(self, dt)
        SetRotation(self, GetRotation(self) + self.rotSpeed * dt)
    end,
})


-- Moves left and right
RegisterComponent("sandboxMover", {
    moveSpeed = 0,
    move = 0,
    startPos = Vec2(0),

    initComponent = function()
        SetFloatProperty("moveSpeed", "Move speed")
    end,

    create = function(self)
        self.startPos = GetPosition(self)
    end,

    update = function(self, dt)
        self.move = self.move + self.moveSpeed * dt
        local pos = Vec2(self.startPos)
        pos.x = pos.x + math.sin(math.rad(self.move)) * 2
        SetPosition(self, pos)

        if IsKeyDown(KEY_SPACE) then
            print("Space Key Down")
        end
    end,
})


-- Destroy target entity when clicked
RegisterComponent("sandboxDestroyButton", {
    target = "",
    targetPosition = Vec2(0),
    destroyed = false,

    initComponent = function()
        SetStringProperty("target", "Entity name to destroy")
    end,

    mouseClick = function(self)
        if not self.destroyed then
            self.targetPosition = GetPosition(self.target)
            Destroy(self.target)
            SetText(self, "Create")
            self.destroyed = true
        else
            -- Recreate it
            local e = CreateEntity(GetRoot())
            SetPosition(e, self.targetPosition)
            SetName(e, self.target)
            
            AddComponent(e, "Sprite")
            SetSpriteTexture(e, "textures/defaultTexture.png")

            local c = AddComponent(e, "sandboxRotator")
            c.scaleSpeed = 360

            SetText(self, "Destroy")
            self.destroyed = false
        end
    end
})


-- Changes color of targeted entity when clicked
RegisterComponent("sandboxColorButton", {
    target = "",
    color = Color(1),

    initComponent = function()
        SetStringProperty("target", "Entity name to change sprite color")
        SetColorProperty("color", "Color to change to")
    end,

    mouseClick = function(self)
        SetSpriteColor(self.target, self.color)
    end
})


-- Stop rotation of targeted entity when clicked (if has sandboxRotator component)
RegisterComponent("sandboxStopRotationButton", {
    target = "",

    initComponent = function()
        SetStringProperty("target", "Entity name to stop rotating/scaling")
    end,

    mouseClick = function(self)
        local c = GetComponent(self.target, "sandboxRotator")
        if c then
            c.rotSpeed = 0
            c.scaleSpeed = 0
        end
    end
})


-- Stop target entity movement (if has sandboxMover component)
RegisterComponent("sandboxStopMover", {
    target = "",

    initComponent = function()
        SetStringProperty("target", "Entity name to stop moving, in a radius of 3")
    end,

    mouseClick = function(self)
        local e = FindEntityByName(self.target, GetPosition(self), 2)
        local c = GetComponent(e, "sandboxMover")
        if c then
            c.moveSpeed = 0
        end
    end
})


-- Keeps a reference on a deleted entity, then tries to access it
RegisterComponent("sandboxBadReference", {
    targetName = "",
    target = nil, -- The entity reference

    initComponent = function()
        SetStringProperty("targetName", "Entity name to keep reference on.")
    end,

    create = function(self)
        self.target = GetEntity(self.targetName)
    end,

    mouseClick = function(self)
        local e = GetEntity(self.target) -- Should return nil
        if e then
            Log("Valid Reference")
        else
            Log("Invalid Reference")
        end
        Log("Target texture = " .. GetSpriteTexture(self.target)) -- Should return empty string if not valid
    end
})

