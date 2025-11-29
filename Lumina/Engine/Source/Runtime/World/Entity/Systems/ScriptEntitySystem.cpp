#include "pch.h"
#include "ScriptEntitySystem.h"

#include "World/Entity/Components/LuaComponent.h"


namespace Lumina
{
    void CScriptEntitySystem::RegisterEventListeners(FSystemContext& SystemContext)
    {
        auto View = SystemContext.CreateView<FLuaScriptsContainerComponent>();
        View.each([&](FLuaScriptsContainerComponent& ScriptContainer)
        {
            for (Scripting::FLuaScriptEntry& Entry : ScriptContainer.Scripts[(uint32)SystemContext.GetUpdateStage()])
            {
                Entry.Environment["ScriptContext"] = &SystemContext;
            }
        });
    }
    
    void CScriptEntitySystem::Update(FSystemContext& SystemContext)
    {
        LUMINA_PROFILE_SCOPE();
        
        auto View = SystemContext.CreateView<FLuaScriptsContainerComponent>();
        View.each([&](FLuaScriptsContainerComponent& ScriptContainer)
        {
            for (Scripting::FLuaScriptEntry& Entry : ScriptContainer.Scripts[(uint32)SystemContext.GetUpdateStage()])
            {
                Entry.Environment["ScriptContext"] = &SystemContext;
                if (sol::optional<sol::function> UpdateFunc = Entry.Table["OnUpdate"])
                {
                    sol::protected_function_result Result = (*UpdateFunc)(Entry.Table);
                
                    if (!Result.valid())
                    {
                        sol::error Error = Result;
                        LOG_INFO("[Sol] Error calling OnUpdate: {}", Error.what());
                    }
                }
            }
        });
    }
}
