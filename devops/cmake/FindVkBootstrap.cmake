include(FetchContent)

FetchContent_Declare(
    vk-bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
    GIT_TAG v1.4.336
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(vk-bootstrap)
