include(FetchContent)

FetchContent_Declare(
    stb
    URL https://github.com/nothings/stb/archive/refs/heads/master.zip
    URL_HASH MD5=aea46e196a83e7948e7595f3c154cabe
)

# No build files in stb as it is a header only library
# Create a variable to access include directory
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)

    if (NOT DEFINED stb_INCLUDE_DIR)
        message(WARNING "stb_INCLUDE_DIR not defined, redefining it")
        set(stb_INCLUDE_DIR "${stb_SOURCE_DIR}")
    endif()

endif()
