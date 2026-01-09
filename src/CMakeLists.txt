#######################################################
# Get all source files
#######################################################
file(GLOB_RECURSE ALL_SOURCE_FILES "*.cpp")

add_executable(vulkan-test
    ${ALL_SOURCE_FILES}
)

add_dependencies(vulkan-test
    compile-shaders
)

target_compile_definitions(vulkan-test PUBLIC
    -DSHADERS_PATH="${CMAKE_BINARY_DIR}/shaders/"
    -DASSETS_PATH="${GIT_REPO_BASE}/assets/"
)

target_include_directories(vulkan-test PRIVATE 
    ${GIT_REPO_BASE}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIR}
    ${stb_INCLUDE_DIR}
)

target_link_libraries(vulkan-test PRIVATE 
    glm::glm
    SDL3::SDL3
    ImGui
    tinyobjloader
    vk-bootstrap
    GPUOpen::VulkanMemoryAllocator
    spdlog::spdlog
    ${Vulkan_LIBRARY}
    fastgltf::fastgltf
)
