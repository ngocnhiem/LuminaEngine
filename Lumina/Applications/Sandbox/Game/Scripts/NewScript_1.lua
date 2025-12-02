

local MySystem = {
    Stages = { UpdateStage.Paused }
}

function MySystem:OnUpdate()

    local view = ScriptContext:View(STransformComponent)

    print(#view)

    for e, c in pairs(view) do
        
        local t = c.STransformComponent
        local tn = t:GetLocation():Normalize()

    end

end

return MySystem