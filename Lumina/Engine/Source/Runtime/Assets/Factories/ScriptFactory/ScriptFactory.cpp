#include "ScriptFactory.h"


namespace Lumina
{
    CObject* CScriptFactory::CreateNew(const FName& Name, CPackage* Package)
    {
        return NewObject<CScriptAsset>(Package, Name);
    }
}
