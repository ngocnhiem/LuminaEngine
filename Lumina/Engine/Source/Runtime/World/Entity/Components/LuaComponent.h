#pragma once
#include "Core/Object/ObjectMacros.h"
#include "Component.h"
#include "LuaComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SLuaComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SLuaComponent)
        

        FName                   TypeName;
        sol::table              LuaTable;
    };
}
