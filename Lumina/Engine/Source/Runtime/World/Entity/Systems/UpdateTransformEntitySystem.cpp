#include "pch.h"
#include "UpdateTransformEntitySystem.h"

#include "Core/Object/Class.h"
#include "glm/gtx/string_cast.hpp"
#include "TaskSystem/TaskSystem.h"
#include "World/Entity/Components/DirtyComponent.h"
#include "World/Entity/Components/RelationshipComponent.h"
#include "World/Entity/Components/RenderComponent.h"
#include "World/Entity/Components/Transformcomponent.h"

namespace Lumina
{
    void CUpdateTransformEntitySystem::Shutdown(FSystemContext& SystemContext)
    {
        
    }

    void CUpdateTransformEntitySystem::Update(FSystemContext& SystemContext)
    {
        LUMINA_PROFILE_SCOPE();

        auto SingleView = SystemContext.CreateView<FNeedsTransformUpdate, STransformComponent>(entt::exclude<SRelationshipComponent>);
        auto RelationshipGroup = SystemContext.CreateGroup<FNeedsTransformUpdate, SRelationshipComponent>(entt::get<STransformComponent>);
        
        if (!RelationshipGroup.empty())
        {
            TVector<entt::entity> DirtyEntities;
            for (auto entity : RelationshipGroup)
            {
                DirtyEntities.push_back(entity);
            }
            
            auto RelationshipTransformCallable = [&](uint32 Index)
            {
                entt::entity DirtyEntity = DirtyEntities[Index];
                
                // First, update the dirty entity itself
                auto& DirtyTransform = RelationshipGroup.get<STransformComponent>(DirtyEntity);
                auto& DirtyRelationship = RelationshipGroup.get<SRelationshipComponent>(DirtyEntity);
                
                if (DirtyRelationship.Parent.IsValid())
                {
                    glm::mat4 ParentWorldTransform         = DirtyRelationship.Parent.GetComponent<STransformComponent>().WorldTransform.GetMatrix();
                    glm::mat4 LocalTransform               = DirtyTransform.Transform.GetMatrix();
                    DirtyTransform.WorldTransform           = FTransform(ParentWorldTransform * LocalTransform);
                }
                else
                {
                    DirtyTransform.WorldTransform = DirtyTransform.Transform;
                }
                
                DirtyTransform.CachedMatrix = DirtyTransform.WorldTransform.GetMatrix();
                
                // Recursively update only the children
                TFunction<void(entt::entity)> UpdateChildrenRecursive;
                UpdateChildrenRecursive = [&](entt::entity ParentEntity)
                {
                    auto& ParentRelationship = SystemContext.Get<SRelationshipComponent>(ParentEntity);
                    
                    for (uint8 i = 0; i < ParentRelationship.NumChildren; ++i)
                    {
                        Entity ChildEntity = ParentRelationship.Children[i];
                        entt::entity ChildHandle = ChildEntity.GetHandle();
                        
                        auto& ParentTransform = SystemContext.Get<STransformComponent>(ParentEntity);
                        auto& ChildTransform = SystemContext.Get<STransformComponent>(ChildHandle);

                        glm::mat4 ParentWorldTransform = ParentTransform.WorldTransform.GetMatrix();
                        glm::mat4 ChildLocalTransform = ChildTransform.Transform.GetMatrix();
                        
                        ChildTransform.WorldTransform = FTransform(ParentWorldTransform * ChildLocalTransform);
                        ChildTransform.CachedMatrix = ChildTransform.WorldTransform.GetMatrix();
                        
                        UpdateChildrenRecursive(ChildHandle);
                    }
                };
                
                // Update all children of the dirty entity
                UpdateChildrenRecursive(DirtyEntity);
            };
            
            // Only schedule tasks if there is a significant amount of transform updates required.
            if (DirtyEntities.size() > 100)
            {
                Task::ParallelFor((uint32)DirtyEntities.size(), (uint32)DirtyEntities.size(), RelationshipTransformCallable);
            }
            else
            {
                for (uint32 i = 0; i < (uint32)DirtyEntities.size(); ++i)
                {
                    RelationshipTransformCallable(i);
                }
            }
        }

        if (SingleView.size_hint() < 1000)
        {
            SingleView.each([&](auto& TU, STransformComponent& TransformComponent)
            {
                TransformComponent.WorldTransform = TransformComponent.Transform;
                TransformComponent.CachedMatrix = TransformComponent.WorldTransform.GetMatrix();  
            });
        }
        else
        {
            std::vector Entities(SingleView.begin(), SingleView.end());

            auto WorkFunctor = [&](FNeedsTransformUpdate& Update, STransformComponent& Transform)
            {
                Transform.WorldTransform = Transform.Transform;
                Transform.CachedMatrix = Transform.Transform.GetMatrix();
            };
            
            Task::ParallelFor(Entities.size(), 64, [&, SingleView](uint32 Index)
            {
                entt::entity Entity = Entities[Index];
                std::apply(WorkFunctor, SingleView.get(Entity));
            });
            
        }
        
        SystemContext.Clear<FNeedsTransformUpdate>();
    }
}
