RegisterComponent("MonsterMgr", {
    create = function(self)
        -- RegisterEvent(self, "onSunSet")
    end,

    onSunSet = function(self, evtData)
        print("Got SunSet event! Extra: " .. evtData.someExtraData)
    end
})
