#include "ScriptEntitySystem.h"

#include "assets/assettypes/script/scriptasset.h"
#include "Scripting/Lua/Scripting.h"

namespace Lumina
{
    void CScriptEntitySystem::PostConstructForWorld(const CWorld* World)
    {
        if (Script.IsValid() == false)
        {
            return;
        }

        DelegateHandle = Script->PostScriptReloaded.AddLambda([this]()
        {
            bReloadRequested = true;
        });
        
        
        OnScriptLoaded();
        
    }

    void CScriptEntitySystem::Initialize(FSystemContext& SystemContext)
    {
        CallLuaFunc(LuaInitialize, SystemContext);
    }

    void CScriptEntitySystem::InitializeEditor(FSystemContext& SystemContext)
    {
        
    }

    void CScriptEntitySystem::WorldBeginPlay(FSystemContext& SystemContext)
    {
        CallLuaFunc(LuaBeginPlay, SystemContext);
    }

    void CScriptEntitySystem::WorldEndPlay(FSystemContext& SystemContext)
    {
        CallLuaFunc(LuaEndPlay, SystemContext);

    }

    void CScriptEntitySystem::Update(FSystemContext& SystemContext)
    {
        LUMINA_PROFILE_SCOPE();

        if (bReloadRequested)
        {
            OnScriptLoaded();
            bReloadRequested = false;
        }
        
        CallLuaFunc(LuaUpdate, SystemContext);
    }

    void CScriptEntitySystem::Shutdown(FSystemContext& SystemContext)
    {
        CallLuaFunc(LuaShutdown, SystemContext);
        Script->PostScriptReloaded.Remove(DelegateHandle);

        Scripting::FScriptingContext::Get().GetState().collect_gc();
    }

    void CScriptEntitySystem::CopyProperties(CEntitySystem* Other)
    {
        CScriptEntitySystem* OtherScriptSystem = (CScriptEntitySystem*)Other;
        Script = OtherScriptSystem->Script;
    }

    sol::protected_function_result CScriptEntitySystem::CallLuaFunc(const sol::protected_function& Function, FSystemContext& Context) const
    {
        if (!Script.IsValid() || !Function.valid())
        {
            return sol::protected_function_result();
        }

        sol::protected_function_result Result = Function(&Context);
        if (!Result.valid())
        {
            sol::error err = Result;
            LOG_ERROR("Lua function call failed!: {}", err.what());
            return sol::protected_function_result();
        }
        return Result;
    }

    void CScriptEntitySystem::OnScriptLoaded()
    {
        sol::state& State = Scripting::FScriptingContext::Get().GetState();
        Environment = sol::environment(State, sol::create, State.globals());
        sol::protected_function_result result = State.safe_script(Script->GetScript().c_str(), Environment);
        
        if (!result.valid())
        {
            sol::error err = result;
            LOG_ERROR("Lua script failed: {}", err.what());
            LuaInitialize   = sol::protected_function();
            LuaBeginPlay    = sol::protected_function();
            LuaUpdate       = sol::protected_function();
            LuaEndPlay      = sol::protected_function();
            LuaShutdown     = sol::protected_function();
            return;
        }

        sol::object InitObj = Environment["Initialize"];
        LuaInitialize = (InitObj.valid() && InitObj.get_type() == sol::type::function) ? sol::protected_function(InitObj) : sol::protected_function();

        sol::object BeginPlayObj = Environment["BeginPlay"];
        LuaBeginPlay = (BeginPlayObj.valid() && BeginPlayObj.get_type() == sol::type::function) ? sol::protected_function(BeginPlayObj) : sol::protected_function();

        sol::object UpdateObj = Environment["Update"];
        LuaUpdate = (UpdateObj.valid() && UpdateObj.get_type() == sol::type::function) ? sol::protected_function(UpdateObj) : sol::protected_function();

        sol::object EndPlayObj = Environment["EndPlay"];
        LuaEndPlay = (EndPlayObj.valid() && EndPlayObj.get_type() == sol::type::function) ? sol::protected_function(EndPlayObj) : sol::protected_function();

        sol::object ShutdownObj = Environment["Shutdown"];
        LuaShutdown = (ShutdownObj.valid() && ShutdownObj.get_type() == sol::type::function) ? sol::protected_function(ShutdownObj) : sol::protected_function();
    }
}
