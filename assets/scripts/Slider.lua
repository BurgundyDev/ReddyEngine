RegisterComponent("Slider", {
    isActive = false,
    value = 1,

    registerOnSlideEvent = function(self, c, functionName)
        RegisterEvent(GetName(self) .. "_onSlide", c, functionName)
    end,

    mouseDown = function(self)
        self.isActive = true
    end,

    mouseUp = function(self)
        self.isActive = false
    end,

    setValue = function(self, val)
        self.value = val
        local bar = GetParent(self)
        local pos = GetPosition(self)
        local w = GetScale(bar).x
        local x = GetPosition(bar).x - w / 2
        pos.x = x + self.value * w;
        SetPosition(self, pos)
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
            local percent = (pos.x - x) / w
            self.value = percent
            SendEvent(GetName(self) .. "_onSlide", percent)
        end
    end
})
