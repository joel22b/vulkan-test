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

    file(GLOB_RECURSE ALL_IMGUI_SOURCE_FILES "${imgui_SOURCE_DIR}/*.cpp")
    
    add_library(ImGui
        ${ALL_IMGUI_SOURCE_FILES}
    )

    target_include_directories(ImGui PUBLIC
        ${imgui_SOURCE_DIR}
    )

endif()
