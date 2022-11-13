-- Hacky button that just scales up when hovered, and down when pressed.
RegisterComponent("Button", {
    originalScale = Vec2(1),
    scale = 1,
    scaleTarget = 1,
    isDown = false,
    isHover = false,
    codeToEval = "",

    initComponent = function(self)
        SetStringProperty("codeToEval", "Code to eval when clicked")
    end,

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
    end,

    mouseClick = function(self)
        if self.codeToEval ~= "" then
            local f = load(self.codeToEval)
            if f then
                f()
            end
        end
    end
})
