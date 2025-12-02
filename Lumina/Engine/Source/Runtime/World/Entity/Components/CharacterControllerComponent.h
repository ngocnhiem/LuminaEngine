#pragma once
#include "Core/Object/ObjectMacros.h"
#include "Component.h"
#include "CharacterControllerComponent.generated.h"


namespace Lumina
{
    REFLECT()
    struct SCharacterControllerComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SCharacterControllerComponent)
        
        PROPERTY(Editable)
        float MouseSensitivity = 0.1f;
        
        PROPERTY(Editable)
        float MinPitch = -89.0f;
        
        PROPERTY(Editable)
        float MaxPitch = 89.0f;
        
        PROPERTY(Editable)
        bool bInvertY = false;
        
        PROPERTY(Editable)
        float EyeHeight = 1.7f;
        
        
        float Yaw = 0.0f;
        float Pitch = 0.0f;


        FUNCTION(Script)
        float GetYaw() const { return Yaw; }

        FUNCTION(Script)
        float GetPitch() const { return Pitch; }


        FUNCTION(Script)
        void UpdateRotation(float mouseDeltaX, float mouseDeltaY)
        {
            Yaw -= mouseDeltaX * MouseSensitivity;
            
            float pitchDelta = mouseDeltaY * MouseSensitivity;
            if (bInvertY)
            {
                pitchDelta = -pitchDelta;
            }

            Pitch = glm::clamp(Pitch + pitchDelta, MinPitch, MaxPitch);
        }


        FUNCTION(Script)
        glm::quat GetRotation() const
        {
            float yawRad = glm::radians(Yaw);
            float pitchRad = glm::radians(Pitch);

            // First pitch around the X axis, then yaw around Y
            glm::quat pitchQuat = glm::angleAxis(pitchRad, glm::vec3(1, 0, 0));
            glm::quat yawQuat = glm::angleAxis(yawRad, glm::vec3(0, 1, 0));

            return yawQuat * pitchQuat;
        }

        FUNCTION(Script)
        glm::vec3 GetForward() const
        {
            float yawRad = glm::radians(Yaw);
            float pitchRad = glm::radians(Pitch);
            
            return glm::vec3(
                glm::cos(pitchRad) * glm::sin(yawRad),
                -glm::sin(pitchRad),
                glm::cos(pitchRad) * glm::cos(yawRad)
            );
        }

        FUNCTION(Script)
        glm::vec3 GetRight() const
        {
            float yawRad = glm::radians(Yaw);
            return glm::vec3(glm::cos(yawRad), 0.0f, -glm::sin(yawRad));
        }

        FUNCTION(Script)
        glm::vec3 GetUp() const
        {
            return glm::cross(GetRight(), GetForward());
        }

        FUNCTION(Script)
        glm::vec3 GetCameraPosition(const glm::vec3& Location) const
        {
            return Location + glm::vec3(0, EyeHeight, 0);
        }
    };
}
