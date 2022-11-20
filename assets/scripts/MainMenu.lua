RegisterComponent("MainMenu", {
    music = "musics/MedievalTavern.ogg",
    menu = "main",

    updateDisplayModeText = function()
        local dm = GetConfig("displayMode")
        local dmText = "Windowed"
        if dm == 1 then
            dmText = "Borderless Fullscreen"
        end
        SetText("btnDisplayMode", "Display Mode - " .. dmText)
    end,

    updateVSyncText = function()
        local vsync = GetConfig("vsync")
        local vsyncText = "On"
        if not vsync then
            vsyncText = "Off"
        end
        SetText("btnVSync", "VSync - " .. vsyncText)
    end,

    initComponent = function()
        SetStringProperty("music", "Music that plays when we enter this state")
    end,

    onMusicVolume = function(self, volume)
        SetConfig("musicVolume", volume)
    end,

    onSFXVolume = function(self, volume)
        SetConfig("sfxVolume", volume)
    end,

    create = function(self)
        self.updateDisplayModeText()
        self.updateVSyncText()
        PlayMusic(self.music)
        RegisterEvent("ShowOptionsMenu", self, "onShowOptionsMenu")
        RegisterEvent("OnDisplayMode", self, "onDisplayMode")
        RegisterEvent("OnVSync", self, "onVSync")
        RegisterEvent("KeyDown", self, "onKeyDown")

        local musicSlider = GetComponent("musicSlider", "Slider")
        musicSlider.setValue(musicSlider, GetConfig("musicVolume"))
        musicSlider.registerOnSlideEvent(musicSlider, self, "onMusicVolume")

        local sfxSlider = GetComponent("sfxSlider", "Slider")
        sfxSlider.setValue(sfxSlider, GetConfig("sfxVolume"))
        sfxSlider.registerOnSlideEvent(sfxSlider, self, "onSFXVolume")
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
        local dm = GetConfig("displayMode")
        if dm == 0 then
            SetConfig("displayMode", 1)
        elseif dm == 1 then
            SetConfig("displayMode", 0)
        end
        self.updateDisplayModeText()
    end,

    onVSync = function(self)
        SetConfig("vsync", not GetConfig("vsync"))
        self.updateVSyncText()
    end
})
