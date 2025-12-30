# Find GLSL Validator program to compile the GLSL shaders to compiled SPIR-V files
find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/bin/ $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

# Find all shader files to compile
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${GIT_REPO_BASE}/shaders/*.frag"
    "${GIT_REPO_BASE}/shaders/*.vert"
    "${GIT_REPO_BASE}/shaders/*.comp"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  message(STATUS "BUILDING SHADER: ${GLSL}")
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(compile-shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)
