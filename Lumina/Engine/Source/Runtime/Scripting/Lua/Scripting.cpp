#include "Scripting.h"

#include "Core/Object/ObjectIterator.h"
#include "Memory/SmartPtr.h"
#include "World/Entity/Components/TagComponent.h"
#include "World/Entity/Systems/SystemContext.h"

namespace Lumina::Scripting
{
    TUniquePtr<FScriptingContext> GScriptingContext;
    
    void Initialize()
    {
        GScriptingContext = MakeUniquePtr<FScriptingContext>();
        GScriptingContext->Initialize();
    }

    void Shutdown()
    {
        GScriptingContext->Shutdown();
        GScriptingContext.reset();
    }

    FScriptingContext& FScriptingContext::Get()
    {
        return *GScriptingContext.get();
    }

    void FScriptingContext::Initialize()
    {
        State.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::coroutine,
            sol::lib::string,
            sol::lib::math,
            sol::lib::table,
            sol::lib::io);

        RegisterCoreTypes();
        
    }

    void FScriptingContext::Shutdown()
    {
    }

    void FScriptingContext::RegisterCoreTypes()
    {
        State.new_usertype<glm::vec2>("vec2",
            sol::call_constructor,
            sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y,
            sol::meta_function::addition, [](const glm::vec2& a, const glm::vec2& b){ return a + b; },
            sol::meta_function::subtraction, [](const glm::vec2& a, const glm::vec2& b){ return a - b; },
            sol::meta_function::multiplication, sol::overload(
                [](const glm::vec2& a, float s){ return a * s; },
                [](const glm::vec2& a, const glm::vec2& b){ return a * b; }
            ),
            sol::meta_function::division, sol::overload(
                [](const glm::vec2& a, float s){ return a / s; },
                [](const glm::vec2& a, const glm::vec2& b){ return a / b; }
            )
        );

        RegisterLuaConverterByName<glm::vec2>("vec2");
        
        State.new_usertype<glm::vec3>("vec3",
            sol::call_constructor,
            sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b){ return a + b; },
            sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b){ return a - b; },
            sol::meta_function::multiplication, sol::overload(
                [](const glm::vec3& a, float s){ return a * s; },
                [](const glm::vec3& a, const glm::vec3& b){ return a * b; }
            ),
            sol::meta_function::division, sol::overload(
                [](const glm::vec3& a, float s){ return a / s; },
                [](const glm::vec3& a, const glm::vec3& b){ return a / b; }
            )
        );

        RegisterLuaConverterByName<glm::vec3>("vec3");

        
        State.new_usertype<glm::vec4>("vec4",
            sol::call_constructor,
            sol::constructors<glm::vec4(), glm::vec4(float, float, float, float)>(),
            "x", &glm::vec4::x,
            "y", &glm::vec4::y,
            "z", &glm::vec4::z,
            "w", &glm::vec4::w,
            sol::meta_function::addition, [](const glm::vec4& a, const glm::vec4& b){ return a + b; },
            sol::meta_function::subtraction, [](const glm::vec4& a, const glm::vec4& b){ return a - b; },
            sol::meta_function::multiplication, sol::overload(
                [](const glm::vec4& a, float s){ return a * s; },
                [](const glm::vec4& a, const glm::vec4& b){ return a * b; }
            ),
            sol::meta_function::division, sol::overload(
                [](const glm::vec4& a, float s){ return a / s; },
                [](const glm::vec4& a, const glm::vec4& b){ return a / b; }
            )
        );

        RegisterLuaConverterByName<glm::vec4>("vec4");
        
        State.new_usertype<glm::quat>("quat",
            sol::call_constructor,
            sol::constructors<glm::quat(), glm::quat(float, float, float, float)>(),
            "x", &glm::quat::x,
            "y", &glm::quat::y,
            "z", &glm::quat::z,
            "w", &glm::quat::w,
            sol::meta_function::addition, [](const glm::quat& a, const glm::quat& b){ return a + b; },
            sol::meta_function::subtraction, [](const glm::quat& a, const glm::quat& b){ return a - b; },
            sol::meta_function::multiplication, sol::overload(
                [](const glm::quat& a, float s){ return a * s; },
                [](const glm::quat& a, const glm::quat& b){ return a * b; }
            )
        );

        RegisterLuaConverterByName<glm::quat>("quat");
        
        State.new_usertype<FTransform>("FTransform",
            sol::call_constructor,
            sol::constructors<FTransform(), FTransform(glm::vec3)>(),
            
            "Location", &FTransform::Location,
            "Rotation", &FTransform::Rotation,
            "Scale",    &FTransform::Scale,

            "Translate",    [](FTransform& Self, const glm::vec3& Translation) { Self.Translate(Translation); },
            "SetLocation",  [](FTransform& Self, const glm::vec3& NewLocation) { Self.Location = NewLocation; },
            "SetRotation",  [](FTransform& Self, const glm::vec3& NewRotation) { Self.SetRotationFromEuler(NewRotation); },
            "SetScale",     [](FTransform& Self, const glm::vec3& NewScale) { Self.SetScale(NewScale); }
        );

        RegisterLuaConverterByName<FTransform>("FTransform");

        State.new_usertype<STagComponent>("STagComponent",
            sol::call_constructor,
            sol::constructors<STagComponent()>(),
            "Tag", &STagComponent::Tag
        );

        RegisterLuaConverterByName<STagComponent>("STagComponent");
        
        FSystemContext::RegisterWithLua(State);
    }

    sol::object FScriptingContext::ConvertToSolObjectFromName(FName Name, const sol::state_view& View, void* Data)
    {
        LUMINA_PROFILE_SCOPE();

        auto it = LuaConverters.find(Name);
        if (it == LuaConverters.end())
        {
            return sol::nil;
        }

        return it->second.To(View, Data);
    }

    void* FScriptingContext::ConvertFromSolObjectToVoidPtrByName(FName Name, const sol::object& Object)
    {
        auto it = LuaConverters.find(Name);
        if (it == LuaConverters.end())
        {
            return nullptr;
        }

        return it->second.From(Object);
    }
}
