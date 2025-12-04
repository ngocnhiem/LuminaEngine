#include "pch.h"
#include "Convert.h"

#include "VulkanCommandList.h"

namespace Lumina::Vk
{
    struct FResourceStateMappingInternal
    {
        EResourceStates State;
        VkPipelineStageFlags2 StageFlags;
        VkAccessFlags2 AccessMask;
        VkImageLayout ImageLayout;

        FResourceStateMapping AsResourceStateMapping() const 
        {
            // It's safe to cast vk::AccessFlags2 -> vk::AccessFlags and vk::PipelineStageFlags2 -> vk::PipelineStageFlags (as long as the enum exist in both versions!),
            // synchronization2 spec says: "The new flags are identical to the old values within the 32-bit range, with new stages and bits beyond that."
            // The below stages are exclusive to synchronization2
            LUM_ASSERT((StageFlags & VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT) != VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT)
            LUM_ASSERT((AccessMask & VK_ACCESS_2_MICROMAP_WRITE_BIT_EXT) != VK_ACCESS_2_MICROMAP_WRITE_BIT_EXT)
            return FResourceStateMapping(State, static_cast<VkPipelineStageFlags>(StageFlags), static_cast<VkAccessFlags>(AccessMask), ImageLayout);
        }

        FResourceStateMapping2 AsResourceStateMapping2() const
        {
            return FResourceStateMapping2(State, StageFlags, AccessMask, ImageLayout);
        }
    };

    static const FResourceStateMappingInternal GResourceStateMap[] =
    {
        { EResourceStates::Common,
            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            0,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::ConstantBuffer,
            VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            VK_ACCESS_2_UNIFORM_READ_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::VertexBuffer,
            VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
            VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::IndexBuffer,
            VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
            VK_ACCESS_2_INDEX_READ_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::IndirectArgument,
            VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,
            VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::ShaderResource,
            VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            VK_ACCESS_2_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
        { EResourceStates::UnorderedAccess,
            VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT,
            VK_IMAGE_LAYOUT_GENERAL },
        { EResourceStates::RenderTarget,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
        { EResourceStates::DepthWrite,
            VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
        { EResourceStates::DepthRead,
            VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL },
        { EResourceStates::StreamOut,
            VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT,
            VK_ACCESS_2_TRANSFORM_FEEDBACK_WRITE_BIT_EXT,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::CopyDest,
            VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            VK_ACCESS_2_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
        { EResourceStates::CopySource,
            VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            VK_ACCESS_2_TRANSFER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL },
        { EResourceStates::ResolveDest,
            VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            VK_ACCESS_2_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
        { EResourceStates::ResolveSource,
            VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            VK_ACCESS_2_TRANSFER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL },
        { EResourceStates::Present,
            VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            0,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR },
        { EResourceStates::AccelStructRead,
            VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::AccelStructWrite,
            VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::AccelStructBuildInput,
            VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::AccelStructBuildBlas,
            VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::ShadingRateSurface,
            VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
            VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR,
            VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR },
        { EResourceStates::OpacityMicromapWrite,
            VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT,
            VK_ACCESS_2_MICROMAP_WRITE_BIT_EXT,
            VK_IMAGE_LAYOUT_UNDEFINED },
        { EResourceStates::OpacityMicromapBuildInput,
            VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT,
            VK_ACCESS_2_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED },
    };

    FResourceStateMappingInternal ConvertResourceStateInternal(EResourceStates state)
    {
        FResourceStateMappingInternal Result = {};

        constexpr uint32 NumStateBits = std::size(GResourceStateMap);

        uint32 StateTmp = uint32(state);
        uint32 BitIndex = 0;

        while (StateTmp != 0 && BitIndex < NumStateBits)
        {
            uint32 Bit = (1 << BitIndex);

            if (StateTmp & Bit)
            {
                const FResourceStateMappingInternal& Mapping = GResourceStateMap[BitIndex];

                Assert(uint32(Mapping.State) == Bit)
                Assert(Result.ImageLayout == VK_IMAGE_LAYOUT_UNDEFINED || Mapping.ImageLayout == VK_IMAGE_LAYOUT_UNDEFINED || Result.ImageLayout == Mapping.ImageLayout)

                Result.State = EResourceStates(Result.State | Mapping.State);
                Result.AccessMask |= Mapping.AccessMask;
                Result.StageFlags |= Mapping.StageFlags;
                if (Mapping.ImageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
                {
                    Result.ImageLayout = Mapping.ImageLayout;
                }

                StateTmp &= ~Bit;
            }

            BitIndex++;
        }

        Assert(Result.State == state)

        return Result;
    }

    FResourceStateMapping2 ConvertResourceState2(EResourceStates State)
    {
        const FResourceStateMappingInternal Mapping = ConvertResourceStateInternal(State);
        return Mapping.AsResourceStateMapping2();
    }
    
    FResourceStateMapping ConvertResourceState(EResourceStates State)
    {
        const FResourceStateMappingInternal Mapping = ConvertResourceStateInternal(State);
        return Mapping.AsResourceStateMapping();
    }
}
