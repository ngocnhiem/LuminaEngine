#pragma once
#include "Component.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "World/Entity/Components/EntityComponentRegistry.h"
#include "World/Entity/Registry/EntityRegistry.h"
#include "TagComponent.generated.h"

namespace Lumina
{
    REFLECT()
    struct LUMINA_API STagComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(STagComponent)

        PROPERTY(Script)
        FName Tag;
    };
}
