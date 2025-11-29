#include "pch.h"
#include "JoltPhysicsScene.h"

#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

#include <algorithm>

#include "JoltPhysics.h"
#include "Core/Profiler/Profile.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "World/Entity/Components/PhysicsComponent.h"
#include "World/World.h"
#include "World/Entity/Components/CharacterComponent.h"
#include "World/Entity/Components/DirtyComponent.h"
#include "World/Entity/Components/TransformComponent.h"

using namespace JPH::literals;

namespace Lumina::Physics
{

    constexpr JPH::EMotionType ToJoltMotionType(EBodyType BodyType)
    {
        switch (BodyType)
        {
            case EBodyType::None:       return JPH::EMotionType::Static;
            case EBodyType::Static:     return JPH::EMotionType::Static;
            case EBodyType::Kinematic:  return JPH::EMotionType::Kinematic;
            case EBodyType::Dynamic:    return JPH::EMotionType::Dynamic;
        }

        LUMINA_NO_ENTRY();
    }

    constexpr JPH::ObjectLayer ToJoltObjectType(EBodyType BodyType)
    {
        if (BodyType == EBodyType::Dynamic || BodyType == EBodyType::Kinematic)
        {
            return Layers::MOVING;
        }

        return Layers::NON_MOVING;
    }
    
    class FObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
        {
            switch (inLayer1)
            {
            case Layers::NON_MOVING:    return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING:        return true;
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
    };

    class FObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
        {
            switch (inObject1)
            {
            case Layers::NON_MOVING: return inObject2 == Layers::MOVING; // Non moving only collides with moving
            case Layers::MOVING: return true; // Moving collides with everything
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
    };

    static FObjectLayerPairFilterImpl GObjectVsObjectLayerFilter;
    static FObjectVsBroadPhaseLayerFilterImpl GObjectVsBroadPhaseLayerFilter;


    void JoltContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
    {
        
    }

    void JoltContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
    {
        ContactListener::OnContactPersisted(inBody1, inBody2, inManifold, ioSettings);
    }

    void JoltContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
    {
        ContactListener::OnContactRemoved(inSubShapePair);
    }

    void JoltContactListener::OverrideFrictionAndRestitution(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
    {
    }

    void JoltContactListener::GetFrictionAndRestitution(const JPH::Body& inBody, const JPH::SubShapeID& inSubShapeID, float& outFriction, float& outRestitution) const
    {
    }

    FJoltPhysicsScene::FJoltPhysicsScene(CWorld* InWorld)
        : World(InWorld)
    {
        JoltSystem = MakeUniquePtr<JPH::PhysicsSystem>();
        JoltInterfaceLayer = MakeUniquePtr<FLayerInterfaceImpl>();
        
        JoltSystem->Init(65536, 0, 131072, 262144, *JoltInterfaceLayer, GObjectVsBroadPhaseLayerFilter, GObjectVsObjectLayerFilter);
        JoltSystem->SetGravity(JPH::Vec3Arg(0.0f, -9.81f, 0.0f));

        JPH::PhysicsSettings JoltSettings;
        JoltSystem->SetPhysicsSettings(JoltSettings);
    }

    FJoltPhysicsScene::~FJoltPhysicsScene()
    {
    }

    void FJoltPhysicsScene::PreUpdate()
    {
        entt::registry& Registry = World->GetEntityRegistry();

        double DeltaTime = GEngine->GetUpdateContext().GetDeltaTime();

        const JPH::BodyLockInterfaceNoLock& LockInterface = JoltSystem->GetBodyLockInterfaceNoLock();
        JPH::BodyInterface& BodyInterface = JoltSystem->GetBodyInterface();
        
        auto BodySyncView = Registry.view<FJoltBodyComponent, STransformComponent, FNeedsTransformUpdate>();
        BodySyncView.each([&](const FJoltBodyComponent& BodyComponent, const STransformComponent& TransformComponent, const FNeedsTransformUpdate&)
        {
            JPH::BodyLockRead Lock(LockInterface, BodyComponent.Body);
            if (Lock.Succeeded())
            {
                const JPH::Body& Body = Lock.GetBody();

                if (Body.IsKinematic())
                {
                    JPH::Vec3 TargetPos(TransformComponent.GetLocation().x, TransformComponent.GetLocation().y, TransformComponent.GetLocation().z);
    
                    JPH::Quat TargetRot(TransformComponent.GetRotation().x, TransformComponent.GetRotation().y, TransformComponent.GetRotation().z, TransformComponent.GetRotation().w);
                    
                    BodyInterface.SetPositionAndRotationWhenChanged(BodyComponent.Body, TargetPos, TargetRot, JPH::EActivation::Activate);
                }
            }
        });
        
        
        auto CharacterView = Registry.view<SCharacterComponent>();
        CharacterView.each([&](SCharacterComponent& CharacterComponent)
        {
            JPH::CharacterVirtual* Character = CharacterComponent.Character;
            
            JPH::CharacterVirtual::EGroundState GroundState = Character->GetGroundState();
            CharacterComponent.bGrounded = (GroundState == JPH::CharacterVirtual::EGroundState::OnGround);

            JPH::Vec3 CurrentVelocity(CharacterComponent.Velocity.x, CharacterComponent.Velocity.y, CharacterComponent.Velocity.z);

            if (!CharacterComponent.bGrounded)
            {
                CharacterComponent.Velocity.y += CharacterComponent.Gravity * (float)DeltaTime;
            }
            else
            {
                JPH::Vec3 GroundVelocity = Character->GetGroundVelocity();

                CharacterComponent.Velocity.y = std::max(CharacterComponent.Velocity.y, 0.0f);

                if (CharacterComponent.bWantsToJump)
                {
                    CharacterComponent.Velocity.y = CharacterComponent.JumpSpeed;
                    CharacterComponent.bWantsToJump = false;
                }

                CurrentVelocity = JPH::Vec3(CharacterComponent.Velocity.x, CharacterComponent.Velocity.y, CharacterComponent.Velocity.z) + GroundVelocity;
            }

            Character->SetLinearVelocity(CurrentVelocity);

            JPH::CharacterVirtual::ExtendedUpdateSettings UpdateSettings;
            UpdateSettings.mStickToFloorStepDown = JPH::Vec3(0.0f, -0.5f, 0.0f);
            UpdateSettings.mWalkStairsStepUp = JPH::Vec3(0.0f, 0.04f, 0.0f);
            

            Character->ExtendedUpdate((float)DeltaTime,
                Character->GetUp() * CharacterComponent.Gravity,
                UpdateSettings,
                JoltSystem->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
                JoltSystem->GetDefaultLayerFilter(Layers::MOVING),
                {},
                {},
                *FJoltPhysicsContext::GetAllocator());
            
        });
    }

    void FJoltPhysicsScene::PostUpdate()
    {
        LUMINA_PROFILE_SCOPE();

        const JPH::BodyLockInterfaceNoLock& LockInterface = JoltSystem->GetBodyLockInterfaceNoLock();
        entt::registry& Registry = World->GetEntityRegistry();

        auto View = Registry.view<FJoltBodyComponent, STransformComponent>();
        View.each([&](entt::entity EntityID, const FJoltBodyComponent& BodyComponent, STransformComponent& TransformComponent)
        {
            const JPH::Body* Body = LockInterface.TryGetBody(BodyComponent.Body);
            
            if (Body == nullptr || !Body->IsActive())
            {
                return;
            }
            
            auto Pos = Body->GetPosition();
            auto Rot = Body->GetRotation();
        
            TransformComponent.SetLocation(glm::vec3(Pos.GetX(), Pos.GetY(), Pos.GetZ()));
            TransformComponent.SetRotation(glm::quat(Rot.GetW(), Rot.GetX(), Rot.GetY(), Rot.GetZ()));
            
            Registry.emplace_or_replace<FNeedsTransformUpdate>(EntityID);   
        });

        auto CharacterView = Registry.view<SCharacterComponent, STransformComponent>();
        CharacterView.each([&](entt::entity Entity, const SCharacterComponent& CharacterComponent, STransformComponent& TransformComponent)
        {
            auto Pos = CharacterComponent.Character->GetPosition();
            auto Rot = CharacterComponent.Character->GetRotation();
        
            TransformComponent.SetLocation(glm::vec3(Pos.GetX(), Pos.GetY(), Pos.GetZ()));
            TransformComponent.SetRotation(glm::quat(Rot.GetW(), Rot.GetX(), Rot.GetY(), Rot.GetZ()));
            
            Registry.emplace_or_replace<FNeedsTransformUpdate>(Entity);   
        });
    }

    void FJoltPhysicsScene::Update(double DeltaTime)
    {
        LUMINA_PROFILE_SCOPE();

        constexpr double MaxDeltaTime = 0.25;
        constexpr int MaxSteps = 5;
    
        DeltaTime = std::min(DeltaTime, MaxDeltaTime);
        Accumulator += DeltaTime;

        CollisionSteps = static_cast<int>(Accumulator / FixedTimeStep);
        CollisionSteps = std::min(CollisionSteps, MaxSteps);

        PreUpdate();

        if (CollisionSteps > 0)
        {
            JoltSystem->Update(FixedTimeStep, CollisionSteps, FJoltPhysicsContext::GetAllocator(), FJoltPhysicsContext::GetThreadPool());
        
            Accumulator -= CollisionSteps * FixedTimeStep;
        
            // Clamp accumulator if we hit max steps
            if (CollisionSteps >= MaxSteps)
            {
                Accumulator = std::min(Accumulator, FixedTimeStep);
            }
        }

        PostUpdate();

    }

    void FJoltPhysicsScene::OnWorldSimulate()
    {
        entt::registry& Registry = World->GetEntityRegistry();

        
        auto View = Registry.view<SRigidBodyComponent, STransformComponent>(entt::exclude<FJoltBodyComponent>);
        
        View.each([&] (entt::entity EntityID, SRigidBodyComponent&, STransformComponent&)
        {
            OnRigidBodyComponentConstructed(Registry, EntityID);
        });

        auto CharacterView = Registry.view<SCharacterComponent>();
        
        CharacterView.each([&] (entt::entity EntityID, SCharacterComponent&)
        {
            OnCharacterComponentConstructed(Registry, EntityID);
        });

        
        
        JoltSystem->OptimizeBroadPhase();


        Registry.on_construct<SCharacterComponent>().connect<&FJoltPhysicsScene::OnCharacterComponentConstructed>(this);
        Registry.on_construct<SRigidBodyComponent>().connect<&FJoltPhysicsScene::OnRigidBodyComponentConstructed>(this);
        Registry.on_destroy<SRigidBodyComponent>().connect<&FJoltPhysicsScene::OnRigidBodyComponentDestroyed>(this);
    }

    void FJoltPhysicsScene::OnWorldStopSimulate()
    {
        entt::registry& Registry = World->GetEntityRegistry();

        Registry.on_construct<SCharacterComponent>().disconnect<&FJoltPhysicsScene::OnCharacterComponentConstructed>(this);

        
        Registry.on_construct<SRigidBodyComponent>().disconnect<&FJoltPhysicsScene::OnRigidBodyComponentConstructed>(this);
        Registry.on_destroy<SRigidBodyComponent>().disconnect<&FJoltPhysicsScene::OnRigidBodyComponentDestroyed>(this);

        auto View = Registry.view<SRigidBodyComponent, FJoltBodyComponent>();
        View.each([&] (entt::entity EntityID, SRigidBodyComponent&, FJoltBodyComponent&)
        {
           OnRigidBodyComponentDestroyed(Registry, EntityID); 
        });
    }

    void FJoltPhysicsScene::OnCharacterComponentConstructed(entt::registry& Registry, entt::entity Entity)
    {
        SCharacterComponent& CharacterComponent = Registry.get<SCharacterComponent>(Entity);
        STransformComponent& TransformComponent = Registry.get<STransformComponent>(Entity);

        JPH::Ref<JPH::CharacterVirtualSettings> Settings = Memory::New<JPH::CharacterVirtualSettings>();
        
        JPH::Ref<JPH::Shape> StandingShape = JPH::RotatedTranslatedShapeSettings(
            JPH::Vec3(0, CharacterComponent.HalfHeight, 0),
            JPH::Quat::sIdentity(),
            Memory::New<JPH::CapsuleShape>(CharacterComponent.HalfHeight, CharacterComponent.Radius * TransformComponent.MaxScale())).Create().Get();

        Settings->mShape = StandingShape;
        Settings->mMass = CharacterComponent.Mass;
        Settings->mMaxStrength = CharacterComponent.MaxStrength;
        Settings->mCharacterPadding = 0.02f;
        Settings->mPenetrationRecoverySpeed = 1.0f;
        Settings->mPredictiveContactDistance = 0.1f;

        Settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), 0.0f);

        JPH::CharacterVirtual* Character = Memory::New<JPH::CharacterVirtual>(Settings,
            JPH::RVec3(TransformComponent.GetLocation().x, TransformComponent.GetLocation().y, TransformComponent.GetLocation().z),
            JPH::Quat(TransformComponent.GetRotation().x, TransformComponent.GetRotation().y, TransformComponent.GetRotation().z, TransformComponent.GetRotation().w),
            0,
            JoltSystem.get());

        CharacterComponent.Character = Character;
    }

    void FJoltPhysicsScene::OnRigidBodyComponentConstructed(entt::registry& Registry, entt::entity EntityID)
    {
        if (!Registry.any_of<SSphereColliderComponent, SBoxColliderComponent>(EntityID))
        {
            LOG_ERROR("Entity {} attempted to construct a rigid body without a collider!");
            return;
        }

        SRigidBodyComponent& RigidBodyComponent = Registry.get<SRigidBodyComponent>(EntityID);
        STransformComponent& TransformComponent = Registry.get<STransformComponent>(EntityID);
        
        JPH::ShapeRefC Shape;

        if (Registry.all_of<SBoxColliderComponent>(EntityID))
        {
            auto& BC = Registry.get<SBoxColliderComponent>(EntityID);
            JPH::BoxShapeSettings Settings(JPH::RVec3(BC.HalfExtent.x * TransformComponent.GetScale().x, BC.HalfExtent.y * TransformComponent.GetScale().y, BC.HalfExtent.z * TransformComponent.GetScale().z));
            Settings.SetEmbedded();
            Shape = Settings.Create().Get();
        }
        else if (Registry.all_of<SSphereColliderComponent>(EntityID))
        {
            auto& SC = Registry.get<SSphereColliderComponent>(EntityID);
            JPH::SphereShapeSettings Settings(SC.Radius * TransformComponent.MaxScale());
            Settings.SetEmbedded();
            Shape = Settings.Create().Get();
        }

        JPH::ObjectLayer Layer = ToJoltObjectType(RigidBodyComponent.BodyType);
        JPH::EMotionType MotionType = ToJoltMotionType(RigidBodyComponent.BodyType);

        glm::quat NormalizedRot = TransformComponent.GetRotation();
        glm::vec3 Position = TransformComponent.GetLocation();
        JPH::Quat JoltQuat = JPH::Quat(NormalizedRot.x, NormalizedRot.y, NormalizedRot.z, NormalizedRot.w);

        JPH::BodyCreationSettings Settings(
            Shape,
            JPH::Vec3(Position.x, Position.y, Position.z),
            JoltQuat,
            MotionType,
            Layer);

        Settings.mRestitution = 0.5f;
        Settings.mFriction = 0.3f;
        Settings.mAngularDamping = RigidBodyComponent.AngularDamping;
        Settings.mLinearDamping = RigidBodyComponent.LinearDamping;

        JPH::BodyInterface& BodyInterface = JoltSystem->GetBodyInterface();
        
        JPH::Body* Body = BodyInterface.CreateBody(Settings);
        BodyInterface.AddBody(Body->GetID(), JPH::EActivation::Activate);
        Registry.emplace<FJoltBodyComponent>(EntityID, Body->GetID());
    }

    void FJoltPhysicsScene::OnRigidBodyComponentDestroyed(entt::registry& Registry, entt::entity EntityID)
    {
        if (FJoltBodyComponent* JoltBodyComponent = Registry.try_get<FJoltBodyComponent>(EntityID))
        {
            JPH::BodyInterface& BodyInterface = JoltSystem->GetBodyInterface();
        
            BodyInterface.RemoveBody(JoltBodyComponent->Body);
            BodyInterface.DestroyBody(JoltBodyComponent->Body);
            Registry.remove<FJoltBodyComponent>(EntityID);
        }
    }

    void FJoltPhysicsScene::OnColliderComponentAdded(entt::registry& Registry, entt::entity EntityID)
    {
    }

    void FJoltPhysicsScene::OnColliderComponentRemoved(entt::registry& Registry, entt::entity EntityID)
    {
    }
}
