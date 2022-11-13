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
    mouseEnter = function(self) print("A:mouseEnter") end,
    mouseLeave = function(self) print("A:mouseLeave") end,
    mouseDown = function(self) print("A:mouseDown") end,
    mouseUp = function(self) print("A:mouseUp") end,
    mouseClick = function(self) print("A:mouseClick") end
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

    destroy = function(self) print("B:destroy") end,
    enable = function(self) print("B:enable") end,
    disable = function(self) print("B:disable") end,
    mouseEnter = function(self) print("B:mouseEnter") end,
    mouseLeave = function(self) print("B:mouseLeave") end,
    mouseDown = function(self) print("B:mouseDown") end,
    mouseUp = function(self) print("B:mouseUp") end,
    mouseClick = function(self) print("B:mouseClick") end
})
