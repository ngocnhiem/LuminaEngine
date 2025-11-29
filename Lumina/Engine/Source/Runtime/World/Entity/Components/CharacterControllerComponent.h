#pragma once
#include "Core/Object/ObjectMacros.h"
#include "Component.h"
#include "CharacterControllerComponent.generated.h"


namespace Lumina
{
    LUM_STRUCT()
    struct SCharacterControllerComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SCharacterControllerComponent)
        
        LUM_PROPERTY(Editable)
        float MouseSensitivity = 0.1f;
        
        LUM_PROPERTY(Editable)
        float MinPitch = -89.0f;
        
        LUM_PROPERTY(Editable)
        float MaxPitch = 89.0f;
        
        LUM_PROPERTY(Editable)
        bool bInvertY = false;
        
        LUM_PROPERTY(Editable)
        float EyeHeight = 1.7f;
        
        
        float Yaw = 0.0f;
        float Pitch = 0.0f;


        float GetYaw() const { return Yaw; }
        float GetPitch() const { return Pitch; }
        
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
        
        glm::quat GetRotation() const
        {
            float yawRad = glm::radians(Yaw);
            float pitchRad = glm::radians(Pitch);

            // First pitch around the X axis, then yaw around Y
            glm::quat pitchQuat = glm::angleAxis(pitchRad, glm::vec3(1, 0, 0));
            glm::quat yawQuat = glm::angleAxis(yawRad, glm::vec3(0, 1, 0));

            return yawQuat * pitchQuat;
        }
        
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
        
        glm::vec3 GetRight() const
        {
            float yawRad = glm::radians(Yaw);
            return glm::vec3(glm::cos(yawRad), 0.0f, -glm::sin(yawRad));
        }
        
        glm::vec3 GetUp() const
        {
            return glm::cross(GetRight(), GetForward());
        }
        
        glm::vec3 GetCameraPosition(const glm::vec3& Location) const
        {
            return Location + glm::vec3(0, EyeHeight, 0);
        }


        static void RegisterLua(sol::state_view State)
        {
            sol::usertype<SCharacterControllerComponent> UserType = State.new_usertype<SCharacterControllerComponent>(
            "SCharacterControllerComponent",
            sol::call_constructor,
            sol::constructors<SCharacterControllerComponent()>(),
            "__type", sol::readonly_property([]() { return "SCharacterControllerComponent"; } ),

            "GetRotation", &SCharacterControllerComponent::GetRotation,
            "GetForward", &SCharacterControllerComponent::GetForward,
            "GetRight", &SCharacterControllerComponent::GetRight,
            "GetUp", &SCharacterControllerComponent::GetUp,
            "UpdateRotation", &SCharacterControllerComponent::UpdateRotation,

            "GetYaw", &SCharacterControllerComponent::GetYaw,
            "GetPitch", &SCharacterControllerComponent::GetPitch
            
            );
        }
    };
}
