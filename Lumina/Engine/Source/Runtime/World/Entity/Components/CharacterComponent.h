#pragma once

#include "Component.h"
#include "Core/Object/ObjectMacros.h"
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include "CharacterComponent.generated.h"


namespace Lumina
{
    LUM_STRUCT()
    struct SCharacterComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SCharacterComponent)
        
        LUM_PROPERTY(Editable)
        float MoveSpeed = 5.0f;

        LUM_PROPERTY(Editable)
        float JumpSpeed = 8.0f;
        
        LUM_PROPERTY(Editable)
        float HalfHeight = 1.8f;

        LUM_PROPERTY(Editable)
        float Radius = 0.3f;
        
        LUM_PROPERTY(Editable)
        float Gravity = -20.0f;

        LUM_PROPERTY(Editable)
        float Mass = 70.0f;
        
        LUM_PROPERTY(Editable)
        float MaxStrength = 100.0f;
        
        LUM_PROPERTY(Editable)
        float MaxSlopeAngle = 45.0f;
        
        LUM_PROPERTY(Editable)
        float StepHeight = 0.4f;

        LUM_PROPERTY(Editable)
        float Acceleration = 10.0f;

        LUM_PROPERTY(Editable)
        float Deceleration = 8.0f;

        LUM_PROPERTY(Editable)
        float AirControl = 0.3f;

        LUM_PROPERTY(Editable)
        float GroundFriction = 8.0f;

        LUM_PROPERTY(Editable)
        int MaxJumpCount = 1;

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
    
        void ApplyFriction(float deltaTime)
        {
            float decel = Deceleration * deltaTime;
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

        void AddVelocity(const glm::vec3& velocity)
        {
            Velocity += velocity;
        }
    
        void SetVelocity(const glm::vec3& velocity)
        {
            Velocity = velocity;
        }

        void Jump()
        {
            if (bGrounded)
            {
                bWantsToJump = true;
            }
        }

        float GetSpeed() const
        {
            return glm::length(glm::vec2(Velocity.x, Velocity.z));
        }

        bool IsMoving() const
        {
            return GetSpeed() > 0.1f;
        }

        bool IsFalling() const
        {
            return !bGrounded && Velocity.y < -0.1f;
        }
    
        bool IsRising() const
        {
            return !bGrounded && Velocity.y > 0.1f;
        }

        bool IsGrounded() const
        {
            LUM_ASSERT(Character)

            return Character->GetGroundState() == JPH::CharacterBase::EGroundState::OnGround;
        }
        
        glm::vec3 GetPosition() const
        {
            JPH::RVec3 pos = Character->GetPosition();
            return glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
        }
    
        void SetPosition(const glm::vec3& position) const
        {
            Character->SetPosition(JPH::RVec3(position.x, position.y, position.z));
        }
    
        void SetRotation(const glm::quat& rotation) const
        {
            Character->SetRotation(JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w));
        }

        static void RegisterLua(sol::state_view State)
        {
            sol::usertype<SCharacterComponent> UserType = State.new_usertype<SCharacterComponent>(
            "SCharacterComponent",
            sol::call_constructor,
            sol::constructors<SCharacterComponent()>(),
            "__type", sol::readonly_property([]() { return "SCharacterComponent"; } ),

            "MoveLocal",        &SCharacterComponent::MoveLocal,
            "ApplyFriction",    &SCharacterComponent::ApplyFriction,
            "AddVelocity",      &SCharacterComponent::AddVelocity,
            "SetVelocity",      &SCharacterComponent::SetVelocity,
            "Jump",             &SCharacterComponent::Jump,
            "GetSpeed",         &SCharacterComponent::GetSpeed,
            "IsMoving",         &SCharacterComponent::IsMoving,
            "IsFalling",        &SCharacterComponent::IsFalling,
            "IsRising",         &SCharacterComponent::IsRising,
            "GetPosition",      &SCharacterComponent::GetPosition,
            "SetRotation",      &SCharacterComponent::SetRotation,
            "IsGrounded",       &SCharacterComponent::IsGrounded

            );
        }
        
        glm::vec3 Velocity;
        bool bGrounded = false;
        bool bWantsToJump = false;

        
        JPH::Ref<JPH::CharacterVirtual> Character;
        
    };
}