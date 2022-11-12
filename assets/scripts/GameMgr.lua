
RegisterComponent("TimeMgr", {
    time = 7,
    timeScale = 60,
    nightTint = Color(0.5, 0.7, 0.9, 1),
    season = "Spring",
    rainDir = Vec2(-0.1, -1),
    daysPerSeason = 7 * 4, -- 4 weeks

    initComponent = function()
        SetFloatProperty("timeScale", "How many real-life seconds per game hours.")
        SetIntProperty("daysPerSeason", "How many days per months.")
        SetStringProperty("season", "Current season. Spring/Summer/Fall/Winter")
        SetColorProperty("nightTint", "Tint to apply to the screen at night.")
        SetVec2Property("rainDir", "Direction the rain falls.")
    end,

    fixedUpdate = function(self, deltatime)
        local prevTime = self.time
        self.time = self.time + (1.0 / self.timeScale) * deltatime

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
