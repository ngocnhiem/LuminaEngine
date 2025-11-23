#include "pch.h"
#include "Scripting.h"

#include "Core/Object/ObjectIterator.h"
#include "Input/Input.h"
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
        SetupInput();
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

    void FScriptingContext::SetupInput()
    {
        sol::table InputTable = State.create_named_table("Input");
        InputTable.set_function("GetMousePosition", &Input::GetMousePosition),
        InputTable.set_function("GetMouseDelta", &Input::GetMouseDelta),
        InputTable.set_function("GetMouseDeltaPitch", &Input::GetMouseDeltaPitch),
        InputTable.set_function("GetMouseDeltaYaw", &Input::GetMouseDeltaYaw),
        InputTable.set_function("EnableCursor", &Input::EnableCursor);
        InputTable.set_function("HideCursor", &Input::HideCursor);
        InputTable.set_function("DisableCursor", &Input::DisableCursor);

        InputTable.set_function("IsKeyDown", [] (uint32 Key) { return Input::IsKeyDown((uint16)Key); });
        InputTable.set_function("IsKeyPressed", [] (uint32 Key) { return Input::IsKeyPressed((uint16)Key); });
        InputTable.set_function("IsKeyReleased", [] (uint32 Key) { return Input::IsKeyReleased((uint16)Key); });
        
        InputTable.new_enum("Key",
            "Space",        Key::Space,
            "Apostrophe",   Key::Apostrophe,
            "Comma",        Key::Comma,
            "Minus",        Key::Minus,
            "Period",       Key::Period,
            "Slash",        Key::Slash,
        
            "D0",           Key::D0,
            "D1",           Key::D1,
            "D2",           Key::D2,
            "D3",           Key::D3,
            "D4",           Key::D4,
            "D5",           Key::D5,
            "D6",           Key::D6,
            "D7",           Key::D7,
            "D8",           Key::D8,
            "D9",           Key::D9,
        
            "Semicolon",    Key::Semicolon,
            "Equal",        Key::Equal,
        
            "A",            Key::A,
            "B",            Key::B,
            "C",            Key::C,
            "D",            Key::D,
            "E",            Key::E,
            "F",            Key::F,
            "G",            Key::G,
            "H",            Key::H,
            "I",            Key::I,
            "J",            Key::J,
            "K",            Key::K,
            "L",            Key::L,
            "M",            Key::M,
            "N",            Key::N,
            "O",            Key::O,
            "P",            Key::P,
            "Q",            Key::Q,
            "R",            Key::R,
            "S",            Key::S,
            "T",            Key::T,
            "U",            Key::U,
            "V",            Key::V,
            "W",            Key::W,
            "X",            Key::X,
            "Y",            Key::Y,
            "Z",            Key::Z,
        
            "LeftBracket",  Key::LeftBracket,
            "Backslash",    Key::Backslash,
            "RightBracket", Key::RightBracket,
            "GraveAccent",  Key::GraveAccent,
        
            "World1",       Key::World1,
            "World2",       Key::World2,
        
            "Escape",       Key::Escape,
            "Enter",        Key::Enter,
            "Tab",          Key::Tab,
            "Backspace",    Key::Backspace,
            "Insert",       Key::Insert,
            "Delete",       Key::Delete,
            "Right",        Key::Right,
            "Left",         Key::Left,
            "Down",         Key::Down,
            "Up",           Key::Up,
            "PageUp",       Key::PageUp,
            "PageDown",     Key::PageDown,
            "Home",         Key::Home,
            "End",          Key::End,
            "CapsLock",     Key::CapsLock,
            "ScrollLock",   Key::ScrollLock,
            "NumLock",      Key::NumLock,
            "PrintScreen",  Key::PrintScreen,
            "Pause",        Key::Pause,
        
            "F1",           Key::F1,
            "F2",           Key::F2,
            "F3",           Key::F3,
            "F4",           Key::F4,
            "F5",           Key::F5,
            "F6",           Key::F6,
            "F7",           Key::F7,
            "F8",           Key::F8,
            "F9",           Key::F9,
            "F10",          Key::F10,
            "F11",          Key::F11,
            "F12",          Key::F12,
            "F13",          Key::F13,
            "F14",          Key::F14,
            "F15",          Key::F15,
            "F16",          Key::F16,
            "F17",          Key::F17,
            "F18",          Key::F18,
            "F19",          Key::F19,
            "F20",          Key::F20,
            "F21",          Key::F21,
            "F22",          Key::F22,
            "F23",          Key::F23,
            "F24",          Key::F24,
            "F25",          Key::F25,
        
            "KP0",          Key::KP0,
            "KP1",          Key::KP1,
            "KP2",          Key::KP2,
            "KP3",          Key::KP3,
            "KP4",          Key::KP4,
            "KP5",          Key::KP5,
            "KP6",          Key::KP6,
            "KP7",          Key::KP7,
            "KP8",          Key::KP8,
            "KP9",          Key::KP9,
            "KPDecimal",    Key::KPDecimal,
            "KPDivide",     Key::KPDivide,
            "KPMultiply",   Key::KPMultiply,
            "KPSubtract",   Key::KPSubtract,
            "KPAdd",        Key::KPAdd,
            "KPEnter",      Key::KPEnter,
            "KPEqual",      Key::KPEqual,
        
            "LeftShift",    Key::LeftShift,
            "LeftControl",  Key::LeftControl,
            "LeftAlt",      Key::LeftAlt,
            "LeftSuper",    Key::LeftSuper,
            "RightShift",   Key::RightShift,
            "RightControl", Key::RightControl,
            "RightAlt",     Key::RightAlt,
            "RightSuper",   Key::RightSuper,
            "Menu",         Key::Menu
        );
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

    
    FLuaConverter::ToFunctionType* FScriptingContext::GetSolConverterFunctionPtrByName(FName Name)
    {
        LUMINA_PROFILE_SCOPE();

        auto it = LuaConverters.find(Name);
        if (it == LuaConverters.end())
        {
            return nullptr;
        }

        return it->second.To;
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
