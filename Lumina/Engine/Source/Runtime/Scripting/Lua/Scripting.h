#pragma once
#include "Core/Object/Class.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include <entt/entt.hpp>
#include "Core/Reflection/Type/Properties/StructProperty.h"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"
#include "World/Entity/Components/Component.h"


namespace Lumina
{
    class CStruct;
}

namespace Lumina::Scripting
{

    struct FLuaConverter
    {
        using ToFunctionType = sol::object(const sol::state_view&, void*);
        using FromFunctionType = void*(const sol::object&);
        
        sol::object (*To) (const sol::state_view&, void*);
        void* (*From) (const sol::object&);
    };

    void Initialize();
    void Shutdown();
    
    class FScriptingContext
    {
    public:
        

        LUMINA_API static FScriptingContext& Get();

        LUMINA_API sol::state& GetState() { return State; }
        LUMINA_API const sol::state& GetState() const { return State; }

        void Initialize();
        void Shutdown();

        void RegisterCoreTypes();
        void SetupInput();
        sol::object ConvertToSolObjectFromName(FName Name, const sol::state_view& View, void* Data);
        FLuaConverter::ToFunctionType* GetSolConverterFunctionPtrByName(FName Name);
        
        void* ConvertFromSolObjectToVoidPtrByName(FName Name, const sol::object& Object);

        
        template<typename T>
        void RegisterEntityComponentStruct();

        template<typename T>
        bool RegisterLuaConverterByName(FName Name);

        
        template<typename T, typename V>
        requires (eastl::is_arithmetic_v<V>)
        void BindFProperty(sol::usertype<T>& UserType, FProperty* Property);

        template<typename T>
        void BindStructFProperty(sol::usertype<T>& UserType, FStructProperty* Property);
    
    private:

        sol::state State;

        THashMap<FName, FLuaConverter> LuaConverters;
        
    };
    
    
    template <typename T>
    void FScriptingContext::RegisterEntityComponentStruct()
    {
        CStruct* StaticStruct = T::StaticStruct();

        if (!RegisterLuaConverterByName<T>(StaticStruct->GetName()))
        {
            return;
        }
        
        if constexpr (requires(sol::state_view V) { T::RegisterLua(V); })
        {
            sol::state_view View(State);
            T::RegisterLua(View);
            return;
        }
        
        FName TypeName = StaticStruct->GetName().c_str();
        
        sol::usertype<T> UserType = State.new_usertype<T>(
            StaticStruct->GetName().c_str(),
            sol::call_constructor,
            sol::constructors<T()>(),
            "__type", sol::readonly_property([TypeName]() { return TypeName.c_str(); } )
        );
        

        StaticStruct->ForEachProperty<FProperty>([&](FProperty* Property)
        {
            switch (Property->GetType())
            {
            case EPropertyTypeFlags::None:
                break;
            case EPropertyTypeFlags::Int8:
                {
                    BindFProperty<T, int8>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::Int16:
                {
                    BindFProperty<T, int16>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::Int32:
                {
                    BindFProperty<T, int32>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::Int64:
                {
                    BindFProperty<T, int64>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::UInt8:
                {
                    BindFProperty<T, uint8>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::UInt16:
                {
                    BindFProperty<T, uint16>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::UInt32:
                {
                    BindFProperty<T, uint32>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::UInt64:
                {
                    BindFProperty<T, uint64>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::Float:
                {
                    BindFProperty<T, float>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::Double:
                {
                    BindFProperty<T, double>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::Bool:
                {
                    BindFProperty<T, bool>(UserType, Property);
                }
                break;
            case EPropertyTypeFlags::Object:
                break;
            case EPropertyTypeFlags::Class:
                break;
            case EPropertyTypeFlags::Name:
                break;
            case EPropertyTypeFlags::String:
                break;
            case EPropertyTypeFlags::Enum:
                break;
            case EPropertyTypeFlags::Vector:
                break;
            case EPropertyTypeFlags::Struct:
                {
                    BindStructFProperty<T>(UserType, static_cast<FStructProperty*>(Property));  
                } 
                break;
            case EPropertyTypeFlags::Count:
                break;
            }
        });
    }

    template <typename T>
    bool FScriptingContext::RegisterLuaConverterByName(FName Name)
    {
        if (LuaConverters.find(Name) != LuaConverters.end())
        {
            LOG_ERROR("Lua Converter Already Registered for type {}", Name);
            return false;
        }
        
        LuaConverters.emplace(Name, FLuaConverter{+[](const sol::state_view& L, void* Data)
        {
            return sol::make_object(L, static_cast<T*>(Data));
        },
        +[](const sol::object& Obj)
        {
            return (void*)&Obj.as<T>();
        }});
        
        return true;
    }

    template <typename T, typename V>
    requires (eastl::is_arithmetic_v<V>)
    void FScriptingContext::BindFProperty(sol::usertype<T>& UserType, FProperty* Property)
    {
        UserType[Property->GetPropertyName().c_str()] = sol::property([Property](T& obj) -> V&
        {
            V* ReturnValue = nullptr;
            Property->GetValue<V>(&obj, ReturnValue, 0);
            return *ReturnValue;
        },
        [Property](T& obj, const V& value)
        {
            Property->SetValue(&obj, value, 0);
        });
    }

    template <typename T>
    void FScriptingContext::BindStructFProperty(sol::usertype<T>& UserType, FStructProperty* Property)
    {
        UserType[Property->GetPropertyName().c_str()] = sol::property([this, Property](T& obj, sol::this_state S) -> sol::table
        {
            sol::state_view Lua(S);
            void* ValuePtr = Property->GetValuePtr<void>(&obj);
            sol::object Object = ConvertToSolObjectFromName(Property->GetStruct()->GetName(), Lua, ValuePtr);
            
            return Object;
        },
        [this, Property](T& obj, const sol::table& Value, sol::this_state S)
        {
            sol::state_view Lua(S);
            void* ValuePtr = Property->GetValuePtr<void>(&obj);
            void* AsValue = ConvertFromSolObjectToVoidPtrByName(Property->GetStruct()->GetName(), Value);
            Memory::Memcpy(ValuePtr, AsValue, Property->GetElementSize());
        });
    }
}
