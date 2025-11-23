#pragma once
#include "World/Entity/Entity.h"


namespace Lumina
{
    class FCameraManager
    {
    public:

        FCameraManager(CWorld* InWorld)
            : World(InWorld)
        {
            
        }
        
        void SetActiveCamera(entt::entity InEntity)
        {
            ActiveCameraEntity = InEntity;
        }
        
        entt::entity GetActiveCameraEntity() const { return ActiveCameraEntity; }
        SCameraComponent& GetCameraComponent() const { return World->GetEntityRegistry().get<SCameraComponent>(ActiveCameraEntity); }
    

    private:

        TObjectPtr<CWorld> World;
        entt::entity ActiveCameraEntity = entt::null;
    
    };
}
