RegisterComponent("MainMenu", {
    music = "musics/MedievalTavern.ogg",
    menu = "main",

    initComponent = function()
        SetStringProperty("music", "Music that plays when we enter this state")
    end,

    create = function(self)
        PlayMusic(self.music)
        RegisterEvent("ShowOptionsMenu", self, "onShowOptionsMenu")
        RegisterEvent("OnDisplayMode", self, "onDisplayMode")
        RegisterEvent("OnVSync", self, "onVSync")
        RegisterEvent("KeyDown", self, "onKeyDown")
    end,

    onKeyDown = function(self, key)
        if key == KEY_ESCAPE then
            self.menu = "main"
            DisableEntity("OptionsMenuRoot")
            EnableEntity("MainMenuRoot")
        end
    end,

    onShowOptionsMenu = function(self)
        self.menu = "options"
        DisableEntity("MainMenuRoot")
        EnableEntity("OptionsMenuRoot")
    end,

    onDisplayMode = function(self)
    end,

    onVSync = function(self)
    end
})
