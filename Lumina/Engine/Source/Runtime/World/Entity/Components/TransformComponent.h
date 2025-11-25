#pragma once

#include "Component.h"
#include "Core/Math/Transform.h"
#include <glm/glm.hpp>
#include "TransformComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API STransformComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(STransformComponent)
        
        STransformComponent() = default;
        STransformComponent(const FTransform& InTransform)
            : Transform(InTransform)
        {}
        
        STransformComponent(const glm::vec3& InPosition)
            :Transform(InPosition)
        {}

        STransformComponent(const glm::mat4& InMatrix)
            : Transform(InMatrix)
        {}
        
        FORCEINLINE FTransform& GetTransform() { return Transform; }
        FORCEINLINE void SetTransform(const FTransform& InTransform) { Transform = InTransform; }

        FORCEINLINE glm::vec3 GetLocation() const    { return Transform.Location; }
        FORCEINLINE glm::quat GetRotation() const    { return Transform.Rotation; }
        FORCEINLINE glm::vec3 GetScale()    const    { return Transform.Scale; }
        FORCEINLINE float MaxScale()        const    { return glm::max(Transform.Scale.x, glm::max(Transform.Scale.y, Transform.Scale.z)); }
        FORCEINLINE glm::mat4 GetMatrix()   const    { return CachedMatrix; }

        FORCEINLINE glm::vec3& Translate(const glm::vec3& Translation)
        {
            Transform.Translate(Translation);
            return Transform.Location;
        }
        
        FORCEINLINE glm::vec3 SetLocation(const glm::vec3& InLocation) 
        { 
            Transform.Location = InLocation;
            return Transform.Location;
        }

        FORCEINLINE glm::quat SetRotation(const glm::quat& InRotation)
        { 
            Transform.Rotation = InRotation;
            return Transform.Rotation;
        }

        FORCEINLINE glm::vec3 SetRotationFromEuler(const glm::vec3& EulerRotation)
        {
            Transform.Rotation = glm::quat(glm::radians(EulerRotation));
            return GetRotationAsEuler();
        }

        FORCEINLINE glm::vec3 SetScale(const glm::vec3& InScale) 
        { 
            Transform.Scale = InScale;
            return Transform.Scale;
        }

        FORCEINLINE glm::vec3 GetRotationAsEuler() const 
        {
            return glm::degrees(glm::eulerAngles(Transform.Rotation));
        }
        
        FORCEINLINE void AddYaw(float Degrees)
        {
            glm::quat yawQuat = glm::angleAxis(glm::radians(Degrees), glm::vec3(0.0f, 1.0f, 0.0f));
            Transform.Rotation = glm::normalize(yawQuat * Transform.Rotation);
        }
        
        FORCEINLINE void AddPitch(float Degrees)
        {
            glm::vec3 Right = Transform.Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
            glm::quat PitchQuat = glm::angleAxis(glm::radians(Degrees), Right);
            Transform.Rotation = glm::normalize(PitchQuat * Transform.Rotation);
        }
        
        FORCEINLINE void AddRoll(float Degrees)
        {
            glm::vec3 Forward = Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::quat rollQuat = glm::angleAxis(glm::radians(Degrees), Forward);
            Transform.Rotation = glm::normalize(rollQuat * Transform.Rotation);
        }
        
        FORCEINLINE void AddRotation(float YawDelta, float PitchDelta, float RollDelta = 0.0f)
        {
            if (YawDelta != 0.0f)
            {
                AddYaw(YawDelta);
            }
            if (PitchDelta != 0.0f)
            {
                AddPitch(PitchDelta);
            }
            if (RollDelta != 0.0f)
            {
                AddRoll(RollDelta);
            }
        }
        
        FORCEINLINE void AddRotationFromMouse(float MouseDeltaX, float MouseDeltaY, float Sensitivity = 0.1f)
        {
            AddYaw(-MouseDeltaX * Sensitivity);
            AddPitch(-MouseDeltaY * Sensitivity);
        }
        
        FORCEINLINE glm::vec3 GetForward() const
        {
            return Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        }
        
        FORCEINLINE glm::vec3 GetRight() const
        {
            return Transform.Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
        }
        
        FORCEINLINE glm::vec3 GetUp() const
        {
            return Transform.Rotation * glm::vec3(0.0f, 1.0f, 0.0f);
        }
        
        static void RegisterLua(sol::state_view State)
        {
            sol::usertype<STransformComponent> UserType = State.new_usertype<STransformComponent>(
            "STransformComponent",
            sol::call_constructor,
            sol::constructors<STransformComponent()>(),
            "__type", sol::readonly_property([]() { return "STransformComponent"; } ),
        
            "Transform", &STransformComponent::Transform,
        
            "GetRotationAsEuler", &STransformComponent::GetRotationAsEuler,
            "Translate",   &STransformComponent::Translate,
            "SetLocation", &STransformComponent::SetLocation,
            "SetRotation", &STransformComponent::SetRotation,
            "SetRotationFromEuler", &STransformComponent::SetRotationFromEuler,
            "SetScale", &STransformComponent::SetScale,
        
            "AddYaw",   &STransformComponent::AddYaw,
            "AddPitch", &STransformComponent::AddPitch,
            "AddRoll",  &STransformComponent::AddRoll,
            "AddRotation", &STransformComponent::AddRotation,
            "AddRotationFromMouse", &STransformComponent::AddRotationFromMouse,
        
            "GetForward", &STransformComponent::GetForward,
            "GetRight",   &STransformComponent::GetRight,
            "GetUp",      &STransformComponent::GetUp
            );
        }
        
    public:

        /** The local transform of an entity */
        LUM_PROPERTY(Editable, Category = "Transform")
        FTransform Transform;

        /** World transform of an entity */
        FTransform WorldTransform = Transform;

        /** The cached matrix always refers to the matrix in world space.*/
        glm::mat4 CachedMatrix = WorldTransform.GetMatrix();
    };
    
}
