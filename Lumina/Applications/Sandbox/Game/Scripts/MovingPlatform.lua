--@type PlatformSystem
local PlatformSystem = {
    Stages = { UpdateStage.PrePhysics }
}

-- per-entity initial positions
local FirstPos = {}
-- per-entity time accumulators if needed
local TimeAcc = {}

function PlatformSystem:OnUpdate()
    local view = ScriptContext:View("MovingCube", STransformComponent)

    for e, component in pairs(view) do
        local transform = component.STransformComponent

        if not FirstPos[e] then
            local pos = transform:GetLocation()
            FirstPos[e] = { x = pos.x, y = pos.y, z = pos.z }
            TimeAcc[e] = 0.0
        end

        TimeAcc[e] = TimeAcc[e] + ScriptContext:GetDeltaTime()

        local start = FirstPos[e]
local speed = 1.5
local phase = (e % 2) * math.pi   -- 0 for even, π for odd
local amplitude = 1.0             -- how high it moves

local offset = math.sin(TimeAcc[e] * speed + phase) * amplitude

x = start.x
y = start.y + offset
z = start.z

transform:SetLocation(vec3(x, y, z))

        ScriptContext:DirtyTransform(e)
    end
end

return PlatformSystem
