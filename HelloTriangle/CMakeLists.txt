project (VulkanHelloTriangleCMake)
cmake_minimum_required (VERSION 3.9.6)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
#file(GLOB HEADERS "include/GLFW/*.h" "include/glm/*.h")
file(GLOB HEADERS "include/*")

add_executable (VulkanHelloTriangleCMake ${SOURCES} ${HEADERS})
target_link_libraries(VulkanHelloTriangleCMake glfw vulkan)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

