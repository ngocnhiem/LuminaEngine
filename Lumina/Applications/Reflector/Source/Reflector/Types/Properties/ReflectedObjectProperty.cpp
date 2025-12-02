#include "ReflectedObjectProperty.h"

namespace Lumina
{
    bool FReflectedObjectProperty::GenerateLuaBinding(eastl::string& Stream)
    {
        Stream += "\t\t\"" + GetDisplayName() + "\", sol::property([]() { })";

        return true;
    }
}
