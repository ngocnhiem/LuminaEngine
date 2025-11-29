#pragma once

#include "Containers/String.h"
#include "Platform/GenericPlatform.h"


namespace Lumina::Scripting
{
    enum class ELuaScriptType : uint8
    {
        System,
        Component,
    };

    struct FLuaScriptEntry
    {
        FString Path;
        sol::environment    Environment;
        sol::table          Table;
        ELuaScriptType      Type;
    };
}
