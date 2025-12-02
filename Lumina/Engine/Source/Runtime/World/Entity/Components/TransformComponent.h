#pragma once

#include "Component.h"
#include "Core/Math/Transform.h"
#include <glm/glm.hpp>
#include "TransformComponent.generated.h"

namespace Lumina
{
    REFLECT()
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

        FUNCTION(Script)
        FORCEINLINE FTransform& GetTransform() { return Transform; }

        FUNCTION(Script)
        FORCEINLINE void SetTransform(const FTransform& InTransform) { Transform = InTransform; }

        FUNCTION(Script)
        FORCEINLINE glm::vec3 GetLocation() const    { return Transform.Location; }

        FUNCTION(Script)
        FORCEINLINE glm::quat GetRotation() const    { return Transform.Rotation; }

        FUNCTION(Script)
        FORCEINLINE glm::vec3 GetScale()    const    { return Transform.Scale; }


        FUNCTION(Script)
        FORCEINLINE float MaxScale()        const    { return glm::max(Transform.Scale.x, glm::max(Transform.Scale.y, Transform.Scale.z)); }

        
        FORCEINLINE glm::mat4 GetMatrix()   const    { return CachedMatrix; }

        FUNCTION(Script)
        FORCEINLINE glm::vec3& Translate(const glm::vec3& Translation)
        {
            Transform.Translate(Translation);
            return Transform.Location;
        }

        FUNCTION(Script)
        FORCEINLINE glm::vec3 SetLocation(const glm::vec3& InLocation) 
        { 
            Transform.Location = InLocation;
            return Transform.Location;
        }

        FUNCTION(Script)
        FORCEINLINE glm::quat SetRotation(const glm::quat& InRotation)
        { 
            Transform.Rotation = InRotation;
            return Transform.Rotation;
        }

        FUNCTION(Script)
        FORCEINLINE glm::vec3 SetRotationFromEuler(const glm::vec3& EulerRotation)
        {
            Transform.Rotation = glm::quat(glm::radians(EulerRotation));
            return GetRotationAsEuler();
        }

        FUNCTION(Script)
        FORCEINLINE glm::vec3 SetScale(const glm::vec3& InScale) 
        { 
            Transform.Scale = InScale;
            return Transform.Scale;
        }

        FUNCTION(Script)
        FORCEINLINE glm::vec3 GetRotationAsEuler() const 
        {
            return glm::degrees(glm::eulerAngles(Transform.Rotation));
        }

        FUNCTION(Script)
        FORCEINLINE void AddYaw(float Degrees)
        {
            glm::quat yawQuat = glm::angleAxis(glm::radians(Degrees), glm::vec3(0.0f, 1.0f, 0.0f));
            Transform.Rotation = glm::normalize(yawQuat * Transform.Rotation);
        }

        FUNCTION(Script)
        FORCEINLINE void AddPitch(float Degrees)
        {
            glm::vec3 Right = Transform.Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
            glm::quat PitchQuat = glm::angleAxis(glm::radians(Degrees), Right);
            Transform.Rotation = glm::normalize(PitchQuat * Transform.Rotation);
        }

        FUNCTION(Script)
        FORCEINLINE void AddRoll(float Degrees)
        {
            glm::vec3 Forward = Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::quat rollQuat = glm::angleAxis(glm::radians(Degrees), Forward);
            Transform.Rotation = glm::normalize(rollQuat * Transform.Rotation);
        }

        FUNCTION(Script)
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

        FUNCTION(Script)
        FORCEINLINE void AddRotationFromMouse(float MouseDeltaX, float MouseDeltaY, float Sensitivity = 0.1f)
        {
            AddYaw(-MouseDeltaX * Sensitivity);
            AddPitch(-MouseDeltaY * Sensitivity);
        }

        FUNCTION(Script)
        FORCEINLINE glm::vec3 GetForward() const
        {
            return Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        }

        FUNCTION(Script)
        FORCEINLINE glm::vec3 GetRight() const
        {
            return Transform.Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
        }

        FUNCTION(Script)
        FORCEINLINE glm::vec3 GetUp() const
        {
            return Transform.Rotation * glm::vec3(0.0f, 1.0f, 0.0f);
        }
        
    public:

        /** The local transform of an entity */
        PROPERTY(Editable, Category = "Transform")
        FTransform Transform;

        /** World transform of an entity */
        FTransform WorldTransform = Transform;

        /** The cached matrix always refers to the matrix in world space.*/
        glm::mat4 CachedMatrix = WorldTransform.GetMatrix();
    };
    
}
