#pragma once

namespace Lumina
{
    class IScriptFactory
    {
    public:

        virtual bool CanHandle(const sol::table& ScriptTable) const = 0;
    };
}