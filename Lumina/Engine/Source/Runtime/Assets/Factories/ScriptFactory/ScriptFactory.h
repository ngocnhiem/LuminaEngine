#pragma once
#include "World/World.h"
#include "Assets/Factories/Factory.h"
#include "core/object/ObjectMacros.h"
#include "Assets/AssetTypes/Script/ScriptAsset.h"
#include "ScriptFactory.generated.h"


namespace Lumina
{
    LUM_CLASS()
    class CScriptFactory : public CFactory
    {
        GENERATED_BODY()
    public:

        CClass* GetSupportedType() const override { return CScriptAsset::StaticClass(); }
        FString GetAssetName() const override { return "Script"; }
        FString GetDefaultAssetCreationName(const FString& InPath) override { return "NewScript"; }

        CObject* CreateNew(const FName& Name, CPackage* Package) override;
    
    };
}
