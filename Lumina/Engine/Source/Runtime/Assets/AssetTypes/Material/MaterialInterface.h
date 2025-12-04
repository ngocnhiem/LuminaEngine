#pragma once
#include "Core/Object/ObjectMacros.h"
#include "Core/Object/Object.h"
#include "Renderer/RHIFwd.h"
#include "MaterialInterface.generated.h"

namespace Lumina
{
    class CMaterial;
    struct FMaterialParameter;
    enum class EMaterialParameterType : uint8;
}

namespace Lumina
{

    REFLECT()
    enum class EMaterialType : uint8
    {
        None,
        PBR,
        PostProcess,
        UI,
    };
    
    REFLECT()
    class LUMINA_API CMaterialInterface : public CObject
    {
        GENERATED_BODY()
    public:

        virtual CMaterial* GetMaterial() const { LUMINA_NO_ENTRY() }
        virtual bool SetVectorValue(const FName& Name, const glm::vec4& Value) { LUMINA_NO_ENTRY() }
        virtual bool SetScalarValue(const FName& Name, const float Value) { LUMINA_NO_ENTRY() }
        virtual bool GetParameterValue(EMaterialParameterType Type, const FName& Name, FMaterialParameter& Param) { LUMINA_NO_ENTRY() }
        
        virtual FRHIBindingSet* GetBindingSet() const { LUMINA_NO_ENTRY() }
        virtual FRHIBindingLayout* GetBindingLayout() const { LUMINA_NO_ENTRY() }
        virtual FRHIVertexShader* GetVertexShader() const { LUMINA_NO_ENTRY() }
        virtual FRHIPixelShader* GetPixelShader() const { LUMINA_NO_ENTRY() }

        virtual EMaterialType GetMaterialType() const { return EMaterialType::None; };

        virtual bool DoesCastShadows() const { return false; };
        virtual bool IsTwoSided() const { return false; };
        virtual bool IsTranslucent() { return false; };
        

        void SetReadyForRender(bool bReady) { bReadyForRender.store(bReady, std::memory_order_release); }
        bool IsReadyForRender() const { return bReadyForRender.load(std::memory_order_acquire); }

    protected:

        std::atomic_bool        bReadyForRender;
        
    };
}
