RegisterComponent("sandboxA", {
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

    create = function(self) print("A:create") end,
    destroy = function(self) print("A:destroy") end,
    enable = function(self) print("A:enable") end,
    disable = function(self) print("A:disable") end,
})


RegisterComponent("sandboxB", {
    moveSpeed = 0,
    move = 0,
    startPos = Vec2(0),

    initComponent = function()
        SetFloatProperty("moveSpeed", "Move speed")
    end,

    create = function(self)
        print("B:create")
        self.startPos = GetPosition(self)
    end,

    update = function(self, dt)
        self.move = self.move + self.moveSpeed * dt
        local pos = Vec2(self.startPos)
        pos.x = pos.x + math.sin(math.rad(self.move)) * 2
        SetPosition(self, pos)
    end,
})


RegisterComponent("sandboxC", {
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
                self.scaleTarget = 1.5
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
    end,

    mouseClick = function(self)
        -- PlaySound("ding.wav")
    end
})
