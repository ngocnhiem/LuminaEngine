#include "pch.h"
#include "SystemContext.h"

#include "sol/sol.hpp"
#include "World/World.h"
#include "World/Entity/Entity.h"
#include "World/Entity/Components/DirtyComponent.h"
#include "World/Entity/Components/LuaComponent.h"
#include "World/Entity/Components/TagComponent.h"

namespace Lumina
{
    FSystemContext::FSystemContext(CWorld* InWorld)
    {
        World = InWorld;
        EntityWorld = &World->EntityWorld;
        EntityWorld->Lock();
        DeltaTime = World->GetWorldDeltaTime();
    }

    FSystemContext::~FSystemContext()
    {
        EntityWorld->Unlock();
    }

    void FSystemContext::RegisterWithLua(sol::state& Lua)
    {
        sol::usertype<FSystemContext> ContextType = Lua.new_usertype<FSystemContext>("SystemContext", sol::no_constructor);
        
        ContextType["DeltaTime"] = sol::readonly_property([](const FSystemContext& Ctx) { return Ctx.GetDeltaTime(); });
        ContextType["Time"] = sol::readonly_property([](const FSystemContext& Ctx) { return Ctx.Time(); });
        
        ContextType["GetTransform"] = [](FSystemContext& Ctx, sol::this_state S, uint32 EntityID)
        {
            return Ctx.Get<STransformComponent>((entt::entity)EntityID);
        };
        
        ContextType["View"] = &FSystemContext::MakeLuaView;

        
        ContextType["DirtyTransform"] = [](FSystemContext& Ctx, uint32 EntityID)
        {
          Ctx.EmplaceOrReplace<FNeedsTransformUpdate>((entt::entity)(EntityID));  
        };

        ContextType["Emplace"] = [](const FSystemContext& Ctx, uint32 EntityID, sol::table Table) -> sol::table
        {
            const char* ComponentName = Table["__type"].get<const char*>();
            entt::hashed_string HashedString = entt::hashed_string(ComponentName);
            
            // This type is resolvable, so we use the emplacement function there.
            if (entt::meta_type Meta = entt::resolve(HashedString))
            {
                using namespace entt::literals;
                void* RegistryPointer = &Ctx.EntityWorld->Registry;
                entt::meta_any NewComponent = Meta.invoke("addcomponent"_hs, {}, (entt::entity)EntityID, RegistryPointer);
                void** Type = NewComponent.try_cast<void*>();
                
                return Scripting::FScriptingContext::Get().ConvertToSolObjectFromName(FName(ComponentName), Table.lua_state(), *Type);
            }

            // This type is not native resolvable, so we're assuming the user created a new lua type.
            
            SLuaComponent& NewComponent = Ctx.EntityWorld->Registry.storage<SLuaComponent>(HashedString).emplace((entt::entity)EntityID);
            
            NewComponent.TypeName = ComponentName;
            NewComponent.LuaTable = Table;

            return NewComponent.LuaTable;
        };


        ContextType["SetActiveCamera"] = [](FSystemContext& Ctx, uint32 EntityID) { Ctx.SetActiveCamera((entt::entity)EntityID); };
    }

    void FSystemContext::SetActiveCamera(entt::entity NewCameraEntity)
    {
        if (Has<true, SCameraComponent>(NewCameraEntity))
        {
            World->SetActiveCamera(NewCameraEntity);
        }
    }

    sol::table FSystemContext::MakeLuaView(sol::variadic_args Types)
    {
        LUMINA_PROFILE_SECTION("FSystemContext::ScriptView");
        using namespace entt::literals;

        entt::runtime_view RuntimeView;

        struct FComponentInfo
        {
            FName Name;
            entt::basic_sparse_set<>* Storage;
            Scripting::FLuaConverter::ToFunctionType* ConversionFunctionPtr;
        };
        
        TFixedVector<FComponentInfo, 4> Storages;
        

        for (auto Arg : Types)
        {
            if (Arg.is<sol::table>())
            {
                sol::table ComponentTable = Arg.as<sol::table>();
                FName LuaName = ComponentTable["__type"].get<const char*>();

                auto HashedComponentName = entt::hashed_string(LuaName.c_str());
                entt::meta_type Meta = entt::resolve(HashedComponentName);
            
                if (entt::basic_sparse_set<>* Storage = EntityWorld->Registry.storage(Meta.info().hash()))
                {
                    Scripting::FLuaConverter::ToFunctionType* FunctionPtr = Scripting::FScriptingContext::Get().GetSolConverterFunctionPtrByName(LuaName);
                    Storages.emplace_back(FComponentInfo{ LuaName, Storage, FunctionPtr });
                    RuntimeView.iterate(*Storage);
                }
            }
            else if (Arg.is<const char*>())
            {
                FName LuaName = Arg.get<const char*>();
                uint32 HashedComponentName = entt::hashed_string(LuaName.c_str());
                if (entt::basic_sparse_set<>* Storage = EntityWorld->Registry.storage(HashedComponentName))
                {
                    Scripting::FLuaConverter::ToFunctionType* FunctionPtr = Scripting::FScriptingContext::Get().GetSolConverterFunctionPtrByName("STagComponent");
                    Storages.emplace_back(FComponentInfo{ LuaName, Storage, FunctionPtr });
                    RuntimeView.iterate(*Storage);
                }
            }
        }
        
        sol::state_view StateView(Types.lua_state());

        int EstimatedSize = (int)RuntimeView.size_hint();
        sol::table ViewTable = StateView.create_table(EstimatedSize);

        uint32 NumEntities = 0;
        int TableSize = 1 + (int)Storages.size();
        RuntimeView.each([&] (entt::entity EntityID)
        {
            NumEntities++;

            sol::table Row = StateView.create_table(0, TableSize);
            Row["Entity"] = (int)EntityID;
            
            for (FComponentInfo& Info : Storages)
            {
                Row[Info.Name.c_str()] = Info.ConversionFunctionPtr(StateView, Info.Storage->value(EntityID));
            }

            ViewTable[(uint32)EntityID] = Row;
        });
        
        return ViewTable;
    }
    
    void FSystemContext::SetActiveCamera(entt::entity New) const
    {
        World->SetActiveCamera(Entity(New, World));
    }

    void FSystemContext::DrawDebugLine(const glm::vec3& Start, const glm::vec3& End, const glm::vec4& Color, float Thickness, float Duration) const
    {
        World->DrawDebugLine(Start, End, Color, Thickness, Duration);
    }

    void FSystemContext::DrawDebugBox(const glm::vec3& Center, const glm::vec3& Extents, const glm::quat& Rotation, const glm::vec4& Color, float Thickness, float Duration) const
    {
        World->DrawDebugBox(Center, Extents, Rotation, Color, Thickness, Duration);
    }

    void FSystemContext::DrawDebugSphere(const glm::vec3& Center, float Radius, const glm::vec4& Color, uint8 Segments, float Thickness, float Duration) const
    {
        World->DrawDebugSphere(Center, Radius, Color, Segments, Thickness, Duration);
    }

    void FSystemContext::DrawDebugCone(const glm::vec3& Apex, const glm::vec3& Direction, float AngleRadians, float Length, const glm::vec4& Color, uint8 Segments, uint8 Stacks, float Thickness, float Duration) const
    {
        World->DrawDebugCone(Apex, Direction, AngleRadians, Length, Color, Segments, Stacks, Thickness, Duration);
    }

    void FSystemContext::DrawFrustum(const glm::mat4& Matrix, float zNear, float zFar, const glm::vec4& Color, float Thickness, float Duration) const
    {
        World->DrawFrustum(Matrix, zNear, zFar, Color, Thickness, Duration);
    }

    void FSystemContext::DrawArrow(const glm::vec3& Start, const glm::vec3& Direction, float Length, const glm::vec4& Color, float Thickness, float Duration, float HeadSize) const
    {
        World->DrawArrow(Start, Direction, Length, Color, Thickness, Duration, HeadSize);
    }

    void FSystemContext::DrawViewVolume(const FViewVolume& ViewVolume, const glm::vec4& Color, float Thickness, float Duration) const
    {
        World->DrawViewVolume(ViewVolume, Color, Thickness, Duration);
    }
}
