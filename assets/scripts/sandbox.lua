RegisterComponent("sandboxA", {
    create = function(self) print("A:create") end,
    destroy = function(self) print("A:destroy") end,
    enable = function(self) print("A:enable") end,
    disable = function(self) print("A:disable") end,
    fixedUpdate = function(self, dt) print("A:fixedUpdate") end,
    update = function(self, dt) print("A:update") end,
    mouseEnter = function(self) print("A:mouseEnter") end,
    mouseLeave = function(self) print("A:mouseLeave") end,
    mouseDown = function(self) print("A:mouseDown") end,
    mouseUp = function(self) print("A:mouseUp") end,
    mouseClick = function(self) print("A:mouseClick") end
})
