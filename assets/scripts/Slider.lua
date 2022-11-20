RegisterComponent("Slider", {
    isActive = false,

    mouseDown = function(self)
        self.isActive = true
    end,

    mouseUp = function(self)
        self.isActive = false
    end,

    update = function(self, dt)
        if self.isActive then
            local bar = GetParent(self)
            local pos = GetPosition(self)
            local w = GetScale(bar).x
            local x = GetPosition(bar).x - w / 2
            local mouse = GetMousePosition()
            pos.x = math.max(x, mouse.x)
            pos.x = math.min(x + w, pos.x)
            SetPosition(self, pos)
        end
    end
})
