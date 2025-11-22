#pragma once

#include "EntityComponentRegistry.h"
#include "Core/Engine/Engine.h"
#include <sol/sol.hpp>
#include "Scripting/Lua/Scripting.h"
#include "entt/entt.hpp"
#include "World/Entity/Registry/EntityRegistry.h"

namespace Lumina
{
#define ENTITY_COMPONENT(Type) \
    static void* GetComponentStructType() { return Type::StaticStruct(); } \
    static void* AddComponent(entt::entity e, void* reg) { return &static_cast<Lumina::FEntityRegistry*>(reg)->emplace_or_replace<Type>(e); } \
    static sol::object ToSolObject(sol::state_view Lua, void* ComponentPtr) { return sol::make_object(Lua, static_cast<Type*>(ComponentPtr)); } \
    static void RegisterMeta() { \
        using namespace entt::literals; \
        entt::meta_factory<Type>(GEngine->GetEngineMetaContext()) \
        .type(#Type ## _hs) \
        .func<&Type::GetComponentStructType>("staticstruct"_hs) \
        .func<&Type::AddComponent>("addcomponent"_hs) \
        .func<&Type::ToSolObject>("tosolobject"_hs); \
        \
        \
        Scripting::FScriptingContext::Get().RegisterEntityComponentStruct<Type>(); \
    } \
    struct DeferredAutoRegister { \
        DeferredAutoRegister() { \
            Lumina::ECS::AddDeferredComponentRegistry(&Type::RegisterMeta); \
        } \
    }; \
    static inline DeferredAutoRegister DeferredAutoRegisterInstance;
}

template<typename T, typename = void>
struct THasSolComponentReflection : std::false_type {};

template<typename T>
struct THasSolComponentReflection<T, std::void_t<decltype(T::RegisterLua(std::declval<sol::state_view>()))>> : std::true_type {};

namespace Lumina::ECS
{
    LUMINA_API inline auto GetSharedMetaCtxHandle()
    {
        return entt::locator<entt::meta_ctx>::handle();
    }

    LUMINA_API inline void SetMetaContext(auto sharedCtx)
    {
        entt::locator<entt::meta_ctx>::reset(sharedCtx);
    }
}
