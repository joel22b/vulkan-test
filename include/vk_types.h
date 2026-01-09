#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <functional>
#include <deque>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <spdlog/spdlog.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err) {                                                      \
            spdlog::get("vulkan-test")->critical("Detected Vulkan error: [{}] on {}:{}", string_VkResult(err), __FILE__, __LINE__); \
            abort();                                                    \
        }                                                               \
    } while (0)


struct AllocatedImage
{
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
};

struct AllocatedBuffer
{
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};

struct Vertex
{
	glm::vec3 position;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
	glm::vec4 color;
};

// holds the resources needed for a mesh
struct GPUMeshBuffers
{
    AllocatedBuffer indexBuffer;
    AllocatedBuffer vertexBuffer;
    VkDeviceAddress vertexBufferAddress;
};

// push constants for our mesh object draws
struct GPUDrawPushConstants
{
    glm::mat4 worldMatrix;
    VkDeviceAddress vertexBuffer;
};


/*******************************************************
 * Formatters
 ******************************************************/
namespace fmt
{

template <>
struct formatter<VkPhysicalDeviceType>
{
    constexpr auto parse(format_parse_context& ctx)
    { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const VkPhysicalDeviceType& type, FormatContext& ctx) const
    {
        std::string typeStr;
        switch (type)
        {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                typeStr = "Other";
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                typeStr = "Integrated GPU";
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                typeStr = "Discrete GPU";
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                typeStr = "Virtual GPU";
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                typeStr = "CPU";
                break;
            default:
                typeStr = "Unknown";
                break;
        }
        return format_to(ctx.out(), "{}", typeStr);
    }
};

} //namespace fmt
