--@type EntitySystemBase
local MovementSystem = {
    Stages = { UpdateStage.PrePhysics }
}

local lastJumpDown = false

function MovementSystem:OnUpdate()
    local deltaTime = ScriptContext:GetDeltaTime()
    
    Input.DisableCursor()

    local view = ScriptContext:View("Player", SCharacterComponent, SCharacterControllerComponent)
    local camera_view = ScriptContext:View("PlayerCamera", SCameraComponent)

    for entity, components in pairs(camera_view) do        
        ScriptContext:SetActiveCamera(entity)
    end


    local jumpPressed = Input.IsKeyDown(Input.Key.Space) and not lastJumpDown

    local mouseDeltaX = Input.GetMouseDeltaX()
    local mouseDeltaY = Input.GetMouseDeltaY()
        
    for entity, components in pairs(view) do

        local character = components.SCharacterComponent
        local controller = components.SCharacterControllerComponent
        
        controller:UpdateRotation(mouseDeltaX, -mouseDeltaY)
        
        local forward = 0.0
        local strafe = 0.0
        
        if Input.IsKeyDown(Input.Key.W) then forward = forward + 1.0 end
        if Input.IsKeyDown(Input.Key.S) then forward = forward - 1.0 end
        if Input.IsKeyDown(Input.Key.A) then strafe = strafe - 1.0 end
        if Input.IsKeyDown(Input.Key.D) then strafe = strafe + 1.0 end

    
        local rotation = controller:GetRotation()
        local PitchQuat = glm.QuatAngleAxis(controller:GetPitch(), vec3(1, 0, 0))
        --character:SetRotation(PitchQuat)

        if forward ~= 0.0 or strafe ~= 0.0 then
            character:MoveLocal(controller:GetRotation(), forward, strafe, deltaTime)
        else
            character:ApplyFriction(deltaTime)
        end
        
        if jumpPressed and character:IsGrounded() then
            character:Jump()
        end
    end

    lastJumpDown = Input.IsKeyDown(Input.Key.Space)
end

return MovementSystem
