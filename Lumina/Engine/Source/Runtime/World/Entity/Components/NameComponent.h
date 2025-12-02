#pragma once

#include "Component.h"
#include "Containers/Name.h"
#include "NameComponent.generated.h"

namespace Lumina
{
    REFLECT()
    struct LUMINA_API SNameComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SNameComponent)

        PROPERTY(Editable)
        FName Name;
    };
}
