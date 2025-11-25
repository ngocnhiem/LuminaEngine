#include "pch.h"
#include "Scripting.h"
#include "Events/KeyCodes.h"
#include "Input/InputProcessor.h"
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
        InputTable.set_function("GetMouseX", [] () { return FInputProcessor::Get().GetMouseX(); }),
        InputTable.set_function("GetMouseY", [] () { return FInputProcessor::Get().GetMouseY(); }),
        InputTable.set_function("GetMouseDeltaX", [] () { return FInputProcessor::Get().GetMouseDeltaX(); }),
        InputTable.set_function("GetMouseDeltaY", [] () { return FInputProcessor::Get().GetMouseDeltaY(); }),
        
        InputTable.set_function("EnableCursor", [] () { FInputProcessor::Get().SetCursorMode(GLFW_CURSOR_NORMAL); }),
        InputTable.set_function("DisableCursor", [] () { FInputProcessor::Get().SetCursorMode(GLFW_CURSOR_DISABLED); }),
        InputTable.set_function("HideCursor", [] () { FInputProcessor::Get().SetCursorMode(GLFW_CURSOR_HIDDEN); }),
        
        InputTable.set_function("IsKeyDown", [] (uint32 Key) { return FInputProcessor::Get().IsKeyDown((EKeyCode)Key); }),
        InputTable.set_function("IsKeyUp", [] (uint32 Key) { return FInputProcessor::Get().IsKeyUp((EKeyCode)Key); }),
        InputTable.set_function("IsKeyRepeated", [] (uint32 Key) { return FInputProcessor::Get().IsKeyRepeated((EKeyCode)Key); }),
        
        InputTable.new_enum("Key",
            "Space",        EKeyCode::Space,
            "Apostrophe",   EKeyCode::Apostrophe,
            "Comma",        EKeyCode::Comma,
            "Minus",        EKeyCode::Minus,
            "Period",       EKeyCode::Period,
            "Slash",        EKeyCode::Slash,
        
            "D0",           EKeyCode::D0,
            "D1",           EKeyCode::D1,
            "D2",           EKeyCode::D2,
            "D3",           EKeyCode::D3,
            "D4",           EKeyCode::D4,
            "D5",           EKeyCode::D5,
            "D6",           EKeyCode::D6,
            "D7",           EKeyCode::D7,
            "D8",           EKeyCode::D8,
            "D9",           EKeyCode::D9,
        
            "Semicolon",    EKeyCode::Semicolon,
            "Equal",        EKeyCode::Equal,
        
            "A",            EKeyCode::A,
            "B",            EKeyCode::B,
            "C",            EKeyCode::C,
            "D",            EKeyCode::D,
            "E",            EKeyCode::E,
            "F",            EKeyCode::F,
            "G",            EKeyCode::G,
            "H",            EKeyCode::H,
            "I",            EKeyCode::I,
            "J",            EKeyCode::J,
            "K",            EKeyCode::K,
            "L",            EKeyCode::L,
            "M",            EKeyCode::M,
            "N",            EKeyCode::N,
            "O",            EKeyCode::O,
            "P",            EKeyCode::P,
            "Q",            EKeyCode::Q,
            "R",            EKeyCode::R,
            "S",            EKeyCode::S,
            "T",            EKeyCode::T,
            "U",            EKeyCode::U,
            "V",            EKeyCode::V,
            "W",            EKeyCode::W,
            "X",            EKeyCode::X,
            "Y",            EKeyCode::Y,
            "Z",            EKeyCode::Z,
        
            "LeftBracket",  EKeyCode::LeftBracket,
            "Backslash",    EKeyCode::Backslash,
            "RightBracket", EKeyCode::RightBracket,
            "GraveAccent",  EKeyCode::GraveAccent,
        
            "World1",       EKeyCode::World1,
            "World2",       EKeyCode::World2,
        
            "Escape",       EKeyCode::Escape,
            "Enter",        EKeyCode::Enter,
            "Tab",          EKeyCode::Tab,
            "Backspace",    EKeyCode::Backspace,
            "Insert",       EKeyCode::Insert,
            "Delete",       EKeyCode::Delete,
            "Right",        EKeyCode::Right,
            "Left",         EKeyCode::Left,
            "Down",         EKeyCode::Down,
            "Up",           EKeyCode::Up,
            "PageUp",       EKeyCode::PageUp,
            "PageDown",     EKeyCode::PageDown,
            "Home",         EKeyCode::Home,
            "End",          EKeyCode::End,
            "CapsLock",     EKeyCode::CapsLock,
            "ScrollLock",   EKeyCode::ScrollLock,
            "NumLock",      EKeyCode::NumLock,
            "PrintScreen",  EKeyCode::PrintScreen,
            "Pause",        EKeyCode::Pause,
        
            "F1",           EKeyCode::F1,
            "F2",           EKeyCode::F2,
            "F3",           EKeyCode::F3,
            "F4",           EKeyCode::F4,
            "F5",           EKeyCode::F5,
            "F6",           EKeyCode::F6,
            "F7",           EKeyCode::F7,
            "F8",           EKeyCode::F8,
            "F9",           EKeyCode::F9,
            "F10",          EKeyCode::F10,
            "F11",          EKeyCode::F11,
            "F12",          EKeyCode::F12,
            "F13",          EKeyCode::F13,
            "F14",          EKeyCode::F14,
            "F15",          EKeyCode::F15,
            "F16",          EKeyCode::F16,
            "F17",          EKeyCode::F17,
            "F18",          EKeyCode::F18,
            "F19",          EKeyCode::F19,
            "F20",          EKeyCode::F20,
            "F21",          EKeyCode::F21,
            "F22",          EKeyCode::F22,
            "F23",          EKeyCode::F23,
            "F24",          EKeyCode::F24,
            "F25",          EKeyCode::F25,
        
            "KP0",          EKeyCode::KP0,
            "KP1",          EKeyCode::KP1,
            "KP2",          EKeyCode::KP2,
            "KP3",          EKeyCode::KP3,
            "KP4",          EKeyCode::KP4,
            "KP5",          EKeyCode::KP5,
            "KP6",          EKeyCode::KP6,
            "KP7",          EKeyCode::KP7,
            "KP8",          EKeyCode::KP8,
            "KP9",          EKeyCode::KP9,
            "KPDecimal",    EKeyCode::KPDecimal,
            "KPDivide",     EKeyCode::KPDivide,
            "KPMultiply",   EKeyCode::KPMultiply,
            "KPSubtract",   EKeyCode::KPSubtract,
            "KPAdd",        EKeyCode::KPAdd,
            "KPEnter",      EKeyCode::KPEnter,
            "KPEqual",      EKeyCode::KPEqual,
        
            "LeftShift",    EKeyCode::LeftShift,
            "LeftControl",  EKeyCode::LeftControl,
            "LeftAlt",      EKeyCode::LeftAlt,
            "LeftSuper",    EKeyCode::LeftSuper,
            "RightShift",   EKeyCode::RightShift,
            "RightControl", EKeyCode::RightControl,
            "RightAlt",     EKeyCode::RightAlt,
            "RightSuper",   EKeyCode::RightSuper,
            "Menu",         EKeyCode::Menu
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
