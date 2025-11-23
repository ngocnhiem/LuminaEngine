#include "pch.h"

#include "InputSubsystem.h"
#include "Core/Profiler/Profile.h"
#include "Core/Windows/Window.h"

namespace Lumina
{
    void FInputSubsystem::Initialize()
    {
    }

    void FInputSubsystem::Update(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();

        FWindow* PrimaryWindow = Windowing::GetPrimaryWindowHandle();
        GLFWwindow* Window = PrimaryWindow->GetWindow();

        int Mode = glfwGetInputMode(Window, GLFW_CURSOR);
        int DesiredMode = Snapshot.CursorMode;
        
        if (Mode != DesiredMode)
        {
            glfwSetInputMode(Window, GLFW_CURSOR, DesiredMode);
        }

        LastSnapshot = Snapshot;
        Snapshot = {};
        Snapshot.CursorMode = DesiredMode;

        // Mouse position and delta
        double xpos, ypos;
        glfwGetCursorPos(Window, &xpos, &ypos);
        glm::vec2 MousePos = { static_cast<float>(xpos), static_cast<float>(ypos) };

        if (!bHasLastMousePos)
        {
            MousePosLastFrame = MousePos;
            bHasLastMousePos = true;
        }

        MouseDelta = MousePos - MousePosLastFrame;
        MousePosLastFrame = MousePos;

        // Keys
        for (int k = Key::Space; k < Key::Num; ++k)
        {
            Snapshot.Keys[k] = glfwGetKey(Window, k);
        }

        // Mouse buttons
        for (int b = 0; b < Mouse::Num; ++b)
        {
            Snapshot.MouseButtons[b] = glfwGetMouseButton(Window, b) == GLFW_PRESS;
        }

        // Cursor position
        Snapshot.MouseX = MousePos.x;
        Snapshot.MouseY = MousePos.y;
    }

    void FInputSubsystem::Deinitialize()
    {
    }

    void FInputSubsystem::SetCursorMode(int NewMode)
    {
        Snapshot.CursorMode = NewMode;
    }

    bool FInputSubsystem::IsKeyDown(KeyCode Key)
    {
        return Snapshot.Keys[Key] == GLFW_PRESS;
    }

    bool FInputSubsystem::IsKeyPressed(KeyCode Key)
    {
        bool bIsDown = Snapshot.Keys[Key] == GLFW_PRESS || Snapshot.Keys[Key] == GLFW_REPEAT;
        bool bWasDown = LastSnapshot.Keys[Key] == GLFW_PRESS || LastSnapshot.Keys[Key] == GLFW_REPEAT;
        return bIsDown && !bWasDown;
    }

    bool FInputSubsystem::IsKeyReleased(KeyCode Key)
    {
        bool bIsDown = Snapshot.Keys[Key] == GLFW_PRESS || Snapshot.Keys[Key] == GLFW_REPEAT;
        bool bWasDown = LastSnapshot.Keys[Key] == GLFW_PRESS || LastSnapshot.Keys[Key] == GLFW_REPEAT;
        return !bIsDown && bWasDown;
    }

    bool FInputSubsystem::IsMouseButtonDown(MouseCode Button)
    {
        return Snapshot.MouseButtons[Button];
    }

    bool FInputSubsystem::IsMouseButtonPressed(MouseCode Button)
    {
        return Snapshot.MouseButtons[Button] && !LastSnapshot.MouseButtons[Button];
    }

    bool FInputSubsystem::IsMouseButtonReleased(MouseCode Button)
    {
        return !Snapshot.MouseButtons[Button] && LastSnapshot.MouseButtons[Button];
    }

    glm::vec2 FInputSubsystem::GetMousePosition() const
    {
        return glm::vec2(Snapshot.MouseX, Snapshot.MouseY);
    }

    glm::vec2 FInputSubsystem::GetMouseDelta() const
    {
        return MouseDelta;
    }
}