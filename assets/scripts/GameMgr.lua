
RegisterComponent("DayTime", {
    time = 7,
    timeScale = 1 / 60,

    initComponent = function()
        SetFloatProperty("timeScale", "1 in game hour per real-life seconds")
    end,

    fixedUpdate = function(self, deltatime)
        local prevTime = self.time
        self.time = self.time + self.timeScale * deltatime

        if prevTime < 20 and self.time >= 20 then
            SendEvent("SunSet")
        end

        -- Check if player been up passed 2am, then we should passout
        if self.time >= 2 then
            -- TODO: Pass out
            self.time = 7 -- wakeup time, 7am

            SendEvent("SunRise")
        end
    end
})
