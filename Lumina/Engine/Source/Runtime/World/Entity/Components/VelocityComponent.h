#pragma once
#include "Component.h"
#include <glm/glm.hpp>

#include "EntityComponentRegistry.h"
#include "VelocityComponent.generated.h"

namespace Lumina
{
    REFLECT()
    struct LUMINA_API SVelocityComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SVelocityComponent);

        PROPERTY(ReadOnly)
        glm::vec3 Velocity;

        PROPERTY(Editable)
        float Speed;
    };
    
}
