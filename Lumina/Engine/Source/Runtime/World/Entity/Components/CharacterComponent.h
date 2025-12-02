#pragma once

#include "Component.h"
#include "Core/Object/ObjectMacros.h"
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include "CharacterComponent.generated.h"


namespace Lumina
{
    REFLECT()
    struct SCharacterComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SCharacterComponent)
        
        PROPERTY(Editable)
        float MoveSpeed = 5.0f;

        PROPERTY(Editable)
        float JumpSpeed = 8.0f;
        
        PROPERTY(Editable)
        float HalfHeight = 1.8f;

        PROPERTY(Editable)
        float Radius = 0.3f;
        
        PROPERTY(Editable)
        float Gravity = -20.0f;

        PROPERTY(Editable)
        float Mass = 70.0f;
        
        PROPERTY(Editable)
        float MaxStrength = 100.0f;
        
        PROPERTY(Editable)
        float MaxSlopeAngle = 45.0f;
        
        PROPERTY(Editable)
        float StepHeight = 0.4f;

        PROPERTY(Editable)
        float Acceleration = 10.0f;

        PROPERTY(Editable)
        float Deceleration = 8.0f;

        PROPERTY(Editable)
        float AirControl = 0.3f;

        PROPERTY(Editable)
        float GroundFriction = 8.0f;

        PROPERTY(Editable)
        int MaxJumpCount = 1;

        FUNCTION(Script)
        void MoveLocal(const glm::quat& rotation, float forward, float strafe, float deltaTime)
        {
            glm::vec3 forwardDir = rotation * glm::vec3(0, 0, -1);
            glm::vec3 rightDir = rotation * glm::vec3(1, 0, 0);
        
            glm::vec3 moveDir = forwardDir * forward + rightDir * strafe;
        
            if (glm::length(moveDir) < 0.001f)
            {
                return;
            }

            moveDir = glm::normalize(moveDir);
            float controlFactor = bGrounded ? 1.0f : AirControl;
            float accel = Acceleration * controlFactor;
        
            glm::vec3 targetVelocity = moveDir * MoveSpeed;
            Velocity.x = glm::mix(Velocity.x, targetVelocity.x, accel * deltaTime);
            Velocity.z = glm::mix(Velocity.z, targetVelocity.z, accel * deltaTime);
        }

        FUNCTION(Script)
        void ApplyFriction(float DeltaTime)
        {
            float decel = Deceleration * DeltaTime;
            float controlFactor = bGrounded ? 1.0f : AirControl;
        
            glm::vec2 horizontalVel(Velocity.x, Velocity.z);
            float speed = glm::length(horizontalVel);
        
            if (speed > 0.001f)
            {
                float newSpeed = glm::max(0.0f, speed - decel * controlFactor);
                glm::vec2 newHorizontal = horizontalVel * (newSpeed / speed);
                Velocity.x = newHorizontal.x;
                Velocity.z = newHorizontal.y;
            }
            else
            {
                Velocity.x = 0.0f;
                Velocity.z = 0.0f;
            }
        }

        FUNCTION(Script)
        void AddVelocity(const glm::vec3& velocity)
        {
            Velocity += velocity;
        }

        FUNCTION(Script)
        void SetVelocity(const glm::vec3& velocity)
        {
            Velocity = velocity;
        }

        FUNCTION(Script)
        void Jump()
        {
            if (bGrounded)
            {
                bWantsToJump = true;
            }
        }

        FUNCTION(Script)
        float GetSpeed() const
        {
            return glm::length(glm::vec2(Velocity.x, Velocity.z));
        }

        FUNCTION(Script)
        bool IsMoving() const
        {
            return GetSpeed() > 0.1f;
        }

        FUNCTION(Script)
        bool IsFalling() const
        {
            return !bGrounded && Velocity.y < -0.1f;
        }

        FUNCTION(Script)
        bool IsRising() const
        {
            return !bGrounded && Velocity.y > 0.1f;
        }

        FUNCTION(Script)
        bool IsGrounded() const
        {
            LUM_ASSERT(Character)

            return Character->GetGroundState() == JPH::CharacterBase::EGroundState::OnGround;
        }

        FUNCTION(Script)
        glm::vec3 GetPosition() const
        {
            JPH::RVec3 pos = Character->GetPosition();
            return glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
        }

        FUNCTION(Script)
        void SetPosition(const glm::vec3& position) const
        {
            Character->SetPosition(JPH::RVec3(position.x, position.y, position.z));
        }

        FUNCTION(Script)
        void SetRotation(const glm::quat& rotation) const
        {
            Character->SetRotation(JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w));
        }
        
        glm::vec3 Velocity;
        bool bGrounded = false;
        bool bWantsToJump = false;

        
        JPH::Ref<JPH::CharacterVirtual> Character;
        
    };
}