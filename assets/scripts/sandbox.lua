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

    create = function(self) print("Rotator:create") end,
    destroy = function(self) print("Rotator:destroy") end,
    enable = function(self) print("Rotator:enable") end,
    disable = function(self) print("Rotator:disable") end,
})


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


RegisterComponent("sandboxButton", {
    originalScale = Vec2(1),
    scale = 1,
    scaleTarget = 1,
    isDown = false,
    isHover = false,

    create = function(self)
        self.originalScale = GetScale(self)
    end,

    update = function(self, dt)
        self.scale = self.scale + (self.scaleTarget - self.scale) * dt * 50;
        SetScale(self, self.originalScale * self.scale)
    end,

    updateScale = function(self)
        if self.isHover then
            if self.isDown then
                self.scaleTarget = 0.8
            else
                self.scaleTarget = 1.2
            end
        else
            self.scaleTarget = 1
        end
    end,

    mouseEnter = function(self)
        self.isHover = true
        self.updateScale(self)
    end,

    mouseLeave = function(self)
        self.isHover = false
        self.updateScale(self)
    end,

    mouseDown = function(self)
        self.isDown = true
        self.updateScale(self)
    end,

    mouseUp = function(self)
        self.isDown = false
        self.updateScale(self)
    end
})


RegisterComponent("sandboxDestroyButton", {
    target = "",

    initComponent = function()
        SetStringProperty("target", "Entity name to destroy")
    end,

    mouseClick = function(self)
        Destroy(self.target)
    end
})


RegisterComponent("sandboxSoundButton", {
    sound = "",

    initComponent = function()
        SetStringProperty("sound", "Sound to play")
    end,

    mouseClick = function(self)
        PlaySound(self.sound)
    end
})


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


RegisterComponent("sandboxStopMover", {
    target = "",

    initComponent = function()
        SetStringProperty("target", "Entity name to stop moving, in a radius of 3")
    end,

    mouseClick = function(self)
        local e = FindEntityByName(self.target, GetPosition(self), 3)
        local c = GetComponent(e, "sandboxMover")
        if c then
            c.moveSpeed = 0
        end
    end
})
