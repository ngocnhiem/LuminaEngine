#pragma once

#include "EntitySystem.h"
#include "Core/Object/ObjectMacros.h"
#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"
#include "scriptentitysystem.generated.h"
#include "Core/Delegates/Delegate.h"

namespace Lumina
{
    class CScriptAsset;
}

namespace Lumina
{
    LUM_CLASS()
    class CScriptEntitySystem : public CEntitySystem
    {
        GENERATED_BODY()
        ENTITY_SYSTEM(CScriptEntitySystem, RequiresUpdate(EUpdateStage::PrePhysics))
    public:
  
        void PostConstructForWorld(CWorld* World) override;
        void WorldBeginPlay(FSystemContext& SystemContext) override;
        void Update(FSystemContext& SystemContext) override;
        void WorldEndPlay(FSystemContext& SystemContext) override;
        void Shutdown(FSystemContext& SystemContext) override;
        void CopyProperties(CEntitySystem* Other) override;


        sol::protected_function_result CallLuaFunc(const sol::protected_function& Function, FSystemContext& Context);
        
        void OnScriptLoaded();



        LUM_PROPERTY(Editable, Category = "Scripts")
        TObjectPtr<CScriptAsset> Script;
        
    private:

        bool                    bReloadRequested = false;
        FDelegateHandle         DelegateHandle;

        sol::environment        Environment;

        sol::protected_function LuaPostConstruct;
        sol::protected_function LuaBeginPlay;
        sol::protected_function LuaUpdate;
        sol::protected_function LuaEndPlay;
        sol::protected_function LuaShutdown;
    };
}
