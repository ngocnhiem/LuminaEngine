#pragma once

#include "Assets/Definition/AssetDefinition.h"
#include "Assets/Factories/ScriptFactory/ScriptFactory.h"
#include "assetdefinition_script.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CAssetDefinition_Script : public CAssetDefinition
    {
        GENERATED_BODY()
    public:

        CFactory* GetFactory() const override { return GetMutableDefault<CScriptFactory>(); }
        CClass* GetAssetClass() const override { return CScriptAsset::StaticClass(); }
        FString GetAssetDisplayName() const override { return "Script"; }
    
    };
}
