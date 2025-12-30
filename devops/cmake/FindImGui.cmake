include(FetchContent)

FetchContent_Declare(
    ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.5
    GIT_SHALLOW TRUE
)

# No build files in ImGui
# Need to create them
FetchContent_GetProperties(ImGui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(ImGui)
    
    add_library(ImGui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    )

    target_include_directories(ImGui
        PUBLIC
            ${imgui_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/backends/
        PRIVATE
            ${Vulkan_INCLUDE_DIR}
    )

    target_link_libraries(ImGui
        PRIVATE
            SDL3::SDL3
    )

endif()
