#pragma once
#include "Core/Object/ObjectMacros.h"
#include "Component.h"
#include "SpringArmComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct SSpringArmComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SSpringArmComponent)

        LUM_PROPERTY(Editable)
        float TargetArmLength = 3.0f;

        LUM_PROPERTY(Editable)
        glm::vec3 SocketOffset;

        LUM_PROPERTY(Editable)
        float ProbeSize = 0.2f;

        LUM_PROPERTY(Editable)
        bool bDoCollisionTest = true;

        LUM_PROPERTY(Editable)
        bool bUseControlRotation = true;
    
    };
}
