RegisterComponent("InGame", {
    music = "musics/8bitKnightMix.ogg",

    initComponent = function()
        SetStringProperty("music", "Music that plays when we enter this state")
    end,

    create = function(self)
        PlayMusic(self.music)
    end
})
