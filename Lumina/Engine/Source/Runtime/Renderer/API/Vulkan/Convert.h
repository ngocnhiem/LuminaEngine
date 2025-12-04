#pragma once

namespace Lumina
{
    enum class EResourceStates : unsigned int;
    struct FResourceStateMapping;
    struct FResourceStateMapping2;
}

namespace Lumina::Vk
{
    FResourceStateMapping ConvertResourceState(EResourceStates State);
    FResourceStateMapping2 ConvertResourceState2(EResourceStates State);

}
