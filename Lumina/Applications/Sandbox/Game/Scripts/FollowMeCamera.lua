--@type EntitySystemBase
local FollowCameraSystem = {
    Stages = { UpdateStage.PostPhysics }
}

local CameraOffset = vec3(0, 2.0, 15.0)
local CameraSmoothSpeed = 15.0

function FollowCameraSystem:OnUpdate()
    local deltaTime = ScriptContext:GetDeltaTime()

    local player_view = ScriptContext:View("Player", SCharacterComponent, SCharacterControllerComponent)
    local camera_view = ScriptContext:View("FollowCamera", STransformComponent)

    for camEntity, camComponents in pairs(camera_view) do
        local cameraTransform = camComponents.STransformComponent

        for playerEntity, playerComponents in pairs(player_view) do
            local character = playerComponents.SCharacterComponent
            local character_controller = playerComponents.SCharacterControllerComponent

            local offsetWorld = character_controller:GetRotation() * CameraOffset
            local targetPos = character:GetPosition() + offsetWorld

            -- Smoothly interpolate position
            local current = cameraTransform:GetLocation()
            local delta = targetPos - current
            local factor = math.min(1.0, CameraSmoothSpeed * deltaTime)
            local newPos = current + delta * factor
            cameraTransform:SetLocation(newPos)

            -- Make camera look at the player
            local direction = newPos - character:GetPosition()
            if glm.Length(direction) > 0.001 then
                direction = glm.Normalize(direction)
                local lookAtRotation = glm.QuatLookAt(direction, vec3(0, 1, 0))
                cameraTransform:SetRotation(lookAtRotation)
            end

            ScriptContext:DirtyTransform(camEntity)
        end
    end
end

return FollowCameraSystem