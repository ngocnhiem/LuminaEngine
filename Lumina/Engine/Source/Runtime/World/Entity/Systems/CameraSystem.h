#pragma once
#include "EntitySystem.h"
#include "CameraSystem.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CCameraSystem : public CEntitySystem
    {
        GENERATED_BODY()
        ENTITY_SYSTEM(RequiresUpdate(US_PostPhysics), RequiresUpdate(US_Paused))
    public:


        void Update(FSystemContext& SystemContext) override;
        
    
    };
}
