#include "pch.h"
#include "CameraSystem.h"
#include "World/Entity/Components/CameraComponent.h"

namespace Lumina
{
    void CCameraSystem::Update(FSystemContext& SystemContext)
    {
        auto View = SystemContext.CreateView<SCameraComponent, STransformComponent>();
        View.each([](SCameraComponent& CameraComponent, const STransformComponent& TransformComponent)
        {
            glm::vec3 UpdatedForward = TransformComponent.WorldTransform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 UpdatedUp      = TransformComponent.WorldTransform.Rotation * glm::vec3(0.0f, 1.0f,  0.0f);
        
            CameraComponent.SetView(TransformComponent.WorldTransform.Location, UpdatedForward, UpdatedUp);
        });
    }
}
