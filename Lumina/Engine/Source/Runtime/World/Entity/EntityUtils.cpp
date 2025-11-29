#include "pch.h"
#include "EntityUtils.h"

#include "Components/RelationshipComponent.h"
#include "components/tagcomponent.h"
#include "Core/Object/Class.h"

namespace Lumina::ECS::Utils
{
    
    bool SerializeEntity(FArchive& Ar, FEntityRegistry& Registry, entt::entity& Entity)
    {
        using namespace entt::literals;
        
        if (Ar.IsWriting())
        {
            Ar << Entity;

            bool bHasRelationship = false;
            int64 SizeBeforeRelationshipFlag = Ar.Tell();
            Ar << bHasRelationship;

            if (FRelationshipComponent* RelationshipComponent = Registry.try_get<FRelationshipComponent>(Entity))
            {
                bHasRelationship = true;
                
                int64 SizeBefore = Ar.Tell();
                Ar.Seek(SizeBeforeRelationshipFlag);
                Ar << bHasRelationship;
                Ar.Seek(SizeBefore);
                
                Ar << RelationshipComponent->Parent;
                Ar << RelationshipComponent->NumChildren;
                
                for (SIZE_T i = 0; i < RelationshipComponent->NumChildren; ++i)
                {
                    Ar << RelationshipComponent->Children[i];
                }
            }

            int64 NumComponentsPos = Ar.Tell();
            SIZE_T NumComponents = 0;
            Ar << NumComponents;

            for (auto [ID, Set] : Registry.storage())
            {
                if (Set.contains(Entity))
                {
                    void* ComponentPointer = Set.value(Entity);
                    if (auto ReturnValue = entt::resolve(Set.type()).invoke("staticstruct"_hs, {}))
                    {
                        void** Type = ReturnValue.try_cast<void*>();
                        if (CStruct* StructType = *(CStruct**)Type)
                        {
                            FName Name = StructType->GetQualifiedName();
                            Ar << Name;
                            
                            int64 ComponentStart = Ar.Tell();

                            int64 ComponentSize = 0;
                            Ar << ComponentSize;

                            int64 StartOfComponentData = Ar.Tell();

                            StructType->SerializeTaggedProperties(Ar, ComponentPointer);

                            int64 EndOfComponentData = Ar.Tell();

                            ComponentSize = EndOfComponentData - StartOfComponentData;

                            Ar.Seek(ComponentStart);
                            Ar << ComponentSize;
                            Ar.Seek(EndOfComponentData);
                            
                            NumComponents++;
                        }
                    }
                }
            }

            int64 SizeBefore = Ar.Tell();
            Ar.Seek(NumComponentsPos);    
            Ar << NumComponents;
            Ar.Seek(SizeBefore);
            
        }
        else if (Ar.IsReading())
        {
            uint32 EntityID = (uint32)Entity;
            Ar << EntityID;
            Entity = (entt::entity)EntityID;

            Entity = Registry.create(Entity);

            bool bHasRelationship = false;
            Ar << bHasRelationship;

            if (bHasRelationship)
            {
                FRelationshipComponent& RelationshipComponent = Registry.emplace<FRelationshipComponent>(Entity);
                Ar << RelationshipComponent.Parent;
                Ar << RelationshipComponent.NumChildren;
                
                for (SIZE_T i = 0; i < RelationshipComponent.NumChildren; ++i)
                {
                    Ar << RelationshipComponent.Children[i];
                }
            }

            SIZE_T NumComponents = 0;
            Ar << NumComponents;

            for (SIZE_T i = 0; i < NumComponents; ++i)
            {
                FName TypeName;
                Ar << TypeName;

                int64 ComponentSize = 0;
                Ar << ComponentSize;

                int64 ComponentStart = Ar.Tell();

                if (CStruct* Struct = FindObject<CStruct>(nullptr, TypeName))
                {
                    using namespace entt::literals;

                    if (Struct == STagComponent::StaticStruct())
                    {
                        STagComponent NewTagComponent;
                        Struct->SerializeTaggedProperties(Ar, &NewTagComponent);

                        Registry.storage<STagComponent>(entt::hashed_string(NewTagComponent.Tag.c_str())).emplace(Entity, NewTagComponent);
                    }
                    else
                    {
                        entt::hashed_string HashString(Struct->GetName().c_str());
                        if (entt::meta_type Meta = entt::resolve(HashString))
                        {
                            void* RegistryPtr = &Registry;
                            
                            if (entt::meta_any RetVal = Meta.invoke("addcomponent"_hs, {}, Entity, RegistryPtr))
                            {
                                if (void** Type = RetVal.try_cast<void*>())
                                {
                                    Struct->SerializeTaggedProperties(Ar, *Type);
                                }
                            }
                        }
                    }
                }

                int64 ComponentEnd = ComponentSize + ComponentStart;
                Ar.Seek(ComponentEnd);
            }
        }
        
        return !Ar.HasError();
    }
    

    bool EntityHasTag(FName Tag, FEntityRegistry& Registry, entt::entity Entity)
    {
        return Registry.storage<STagComponent>(entt::hashed_string(Tag.c_str())).contains(Entity);
    }
}
