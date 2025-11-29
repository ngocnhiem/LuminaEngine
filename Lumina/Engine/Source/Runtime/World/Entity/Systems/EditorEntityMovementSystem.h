#pragma once
#include "EntitySystem.h"
#include "EditorEntityMovementSystem.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CEditorEntityMovementSystem : public CEntitySystem
    {
        GENERATED_BODY()
        ENTITY_SYSTEM(RequiresUpdate(EUpdateStage::Paused))
    public:
        

        void Update(FSystemContext& SystemContext) override;
        
    };
}
