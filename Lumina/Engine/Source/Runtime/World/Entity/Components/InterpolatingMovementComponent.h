#pragma once
#include "Component.h"
#include "Core/Object/ManualReflectTypes.h"
#include "Core/Math/Sine.h"
#include "InterpolatingMovementComponent.generated.h"

namespace Lumina
{
    
    LUM_STRUCT()
    struct LUMINA_API SInterpolatingMovementComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SInterpolatingMovementComponent)

        LUM_PROPERTY(Editable, Category = "Interp")
        FTransform Start;
        
        LUM_PROPERTY(Editable, Category = "Interp")
        FTransform End;

        LUM_PROPERTY(Editable, Category = "Interp")
        float Speed = 1.0f;

        FTransform OriginTransform;
        float Alpha = 0.0f;
        bool bForward = true;
    };

    LUM_STRUCT()
    struct LUMINA_API SSineWaveMovementComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SSineWaveMovementComponent)
    
        // Wave Parameters
        LUM_PROPERTY(Editable, Category = "Wave")
        glm::vec3 Axis = glm::vec3(0.0f, 1.0f, 0.0f);
    
        LUM_PROPERTY(Editable, Category = "Wave")
        float Amplitude = 1.0f;
    
        LUM_PROPERTY(Editable, Category = "Wave")
        float Frequency = 1.0f;
    
        LUM_PROPERTY(Editable, Category = "Wave")
        float PhaseOffset = 0.0f;
    
        // Optional Multi-Axis Movement
        LUM_PROPERTY(Editable, Category = "Multi-Axis")
        bool bEnableSecondaryAxis = false;
    
        LUM_PROPERTY(Editable, Category = "Multi-Axis")
        glm::vec3 SecondaryAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    
        LUM_PROPERTY(Editable, Category = "Multi-Axis")
        float SecondaryAmplitude = 0.5f;
    
        LUM_PROPERTY(Editable, Category = "Multi-Axis")
        float SecondaryFrequency = 1.5f;
    
        LUM_PROPERTY(Editable, Category = "Multi-Axis")
        float SecondaryPhaseOffset = 0.0f;
    
        // Wave Modifiers
        LUM_PROPERTY(Editable, Category = "Modifiers")
        ESineWaveType WaveType = ESineWaveType::Sine;
    
        LUM_PROPERTY(Editable, Category = "Modifiers")
        bool bDampingEnabled = false;
    
        LUM_PROPERTY(Editable, Category = "Modifiers")
        float DampingFactor = 0.1f;
    
        LUM_PROPERTY(Editable, Category = "Modifiers")
        bool bAmplitudeModulation = false;
    
        LUM_PROPERTY(Editable, Category = "Modifiers")
        float AmplitudeModulationFrequency = 0.5f;
    
        // Behavior
        LUM_PROPERTY(Editable, Category = "Behavior")
        bool bUseRelativeMovement = true;
    
        LUM_PROPERTY(Editable, Category = "Behavior")
        bool bAutoStart = true;
    
        LUM_PROPERTY(Editable, Category = "Behavior")
        float StartDelay = 0.0f;
    
        glm::vec3 InitialPosition = glm::vec3(0.0f);
        float CurrentTime = 0.0f;
        float CurrentAmplitude = 0.0f;
        bool bInitialized = false;
        bool bIsActive = false;
    };
    
}
