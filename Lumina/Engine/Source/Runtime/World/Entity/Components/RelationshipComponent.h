#pragma once

#include "Module/API.h"
#include "Containers/Array.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    struct LUMINA_API FRelationshipComponent
    {
        constexpr static uint32 MaxChildren = 32;

        
        entt::entity                        Parent = entt::null;
        uint32                              NumChildren = 0;
        
        TArray<entt::entity, MaxChildren>   Children {};
    };

    struct FParentEntityTag { };
    struct FChildEntityTag { };
}
