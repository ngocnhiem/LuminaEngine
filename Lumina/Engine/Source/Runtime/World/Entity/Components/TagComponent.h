#pragma once
#include "Core/Object/ObjectMacros.h"
#include "Component.h"
#include "TagComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API STagComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(STagComponent)

        LUM_PROPERTY()
        FName Tag;
    };
}
