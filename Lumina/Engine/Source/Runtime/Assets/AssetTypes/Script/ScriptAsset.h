#pragma once
#include "Core/Object/Object.h"
#include "Core/Delegates/Delegate.h"
#include "ScriptAsset.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CScriptAsset : public CObject
    {
        GENERATED_BODY()
    public:

        bool IsAsset() const override { return true; }
        
        const FString& GetScript() const { return Script; }
        

        LUM_PROPERTY()
        FString Script;

        TMulticastDelegate<void> PostScriptReloaded;
    };
}
