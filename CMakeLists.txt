# CMakeList.txt : CMake project for isRealEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project ("isRealEngine")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB headerFiles ${CMAKE_SOURCE_DIR}/include/*.h)



aux_source_directory(${CMAKE_SOURCE_DIR}/src engine_src)
# Add source to this project's executable.
add_executable (isRealEngine ${engine_src} ${headerFiles})

target_include_directories(isRealEngine PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_features(isRealEngine PRIVATE cxx_std_20)

if (MSVC)
    add_compile_options(std:c++20)
    target_compile_options(isRealEngine PUBLIC "/ZI")
    target_link_options(isRealEngine PUBLIC "/INCREMENTAL")
endif()



set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


# target_include_directories(isRealEngine PRIVATE include)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET isRealEngine PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
find_package(nlohmann_json CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)


target_link_libraries(isRealEngine PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(isRealEngine PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(isRealEngine PRIVATE glm::glm)

find_package(Vulkan REQUIRED)
target_link_libraries(isRealEngine PRIVATE Vulkan::Vulkan)

find_package(Vulkan) # https://cmake.org/cmake/help/latest/module/FindVulkan.html, CMake 3.21+
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(isRealEngine PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
target_include_directories(isRealEngine PRIVATE ${TINYGLTF_INCLUDE_DIRS})
