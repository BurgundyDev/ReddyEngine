-- Component that will destroy it's host entity in x amount of time
RegisterComponent("DestroyTimeout", {
    timeout = 5,

    initComponent = function()
        SetFloatProperty("timeout", "Timeout before destroying entity in seconds.")
    end,

    update = function(self, dt)
        self.timeout = self.timeout - dt
        if self.timeout <= 0 then
            Destroy(self)
        end
    end
})
