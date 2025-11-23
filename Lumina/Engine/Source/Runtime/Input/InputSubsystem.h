#pragma once

#include "Subsystems/Subsystem.h"
#include <glm/glm.hpp>
#include <glfw/glfw3.h>
#include "Events/KeyCodes.h"
#include "Events/MouseCodes.h"

namespace Lumina
{
    class FInputSubsystem : public ISubsystem
    {
    public:

        struct FInputSnapshot
        {
            TArray<int, (uint32)Key::Num> Keys = {};
            TArray<bool, (uint32)Mouse::Num> MouseButtons = {};
            
            float MouseX = 0.0f;
            float MouseY = 0.0f;
            
            int CursorMode = GLFW_CURSOR_NORMAL;
        };
        
        void Initialize() override;
        void Update(const FUpdateContext& UpdateContext);
        void Deinitialize() override;

        LUMINA_API void SetCursorMode(int NewMode);

        // Key input
        LUMINA_API bool IsKeyDown(KeyCode Key);
        LUMINA_API bool IsKeyPressed(KeyCode Key);   // True only on the frame the key went down
        LUMINA_API bool IsKeyReleased(KeyCode Key);  // True only on the frame the key went up

        // Mouse button input
        LUMINA_API bool IsMouseButtonDown(MouseCode Button);
        LUMINA_API bool IsMouseButtonPressed(MouseCode Button);
        LUMINA_API bool IsMouseButtonReleased(MouseCode Button);

        // Mouse position
        LUMINA_API glm::vec2 GetMousePosition() const;
        LUMINA_API glm::vec2 GetMouseDelta() const;
        LUMINA_API float GetMouseDeltaX() const { return MouseDelta.x; }
        LUMINA_API float GetMouseDeltaY() const { return MouseDelta.y; }
        
    private:

        glm::vec2 MouseDelta = glm::vec2(0.0f);
        glm::vec2 MousePosLastFrame = glm::vec2(0.0f);
        bool bHasLastMousePos = false;

        FInputSnapshot Snapshot;
        FInputSnapshot LastSnapshot;
    };
}