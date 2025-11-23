#pragma once
#include "InputSubsystem.h"
#include "Events/KeyCodes.h"

namespace Lumina
{
    namespace Input
    {
        LUMINA_API inline bool IsKeyPressed(KeyCode Key)
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->IsKeyPressed(Key);
        }

        LUMINA_API inline void DisableCursor()
        {
            GEngine->GetEngineSubsystem<FInputSubsystem>()->SetCursorMode(GLFW_CURSOR_DISABLED);
        }

        LUMINA_API inline void HideCursor()
        {
            GEngine->GetEngineSubsystem<FInputSubsystem>()->SetCursorMode(GLFW_CURSOR_HIDDEN);
        }

        LUMINA_API inline void EnableCursor()
        {
            GEngine->GetEngineSubsystem<FInputSubsystem>()->SetCursorMode(GLFW_CURSOR_NORMAL);
        }
        
        LUMINA_API inline bool IsKeyDown(KeyCode Key)
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->IsKeyDown(Key);
        }

        LUMINA_API inline bool IsKeyReleased(KeyCode Key)
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->IsKeyReleased(Key);
        }
        
        LUMINA_API inline bool IsMouseButtonPressed(MouseCode Button)
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->IsMouseButtonPressed(Button);
        }

        LUMINA_API inline bool IsMouseButtonDown(MouseCode Button)
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->IsMouseButtonDown(Button);
        }
        
        LUMINA_API inline bool IsMouseButtonReleased(MouseCode Button)
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->IsMouseButtonReleased(Button);
        }

        LUMINA_API inline glm::vec2 GetMousePosition()
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->GetMousePosition();
        }

        LUMINA_API inline glm::vec2 GetMouseDelta()
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->GetMouseDelta();
        }

        LUMINA_API inline float GetMouseDeltaPitch()
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->GetMouseDeltaY();
        }

        LUMINA_API inline float GetMouseDeltaYaw()
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->GetMouseDeltaX();
        }
        
    }
}
