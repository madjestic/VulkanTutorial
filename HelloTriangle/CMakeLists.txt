project (HelloTriangle)
cmake_minimum_required (VERSION 3.9.6)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*")

add_executable (HelloTriangle ${SOURCES} ${HEADERS})
target_link_libraries(HelloTriangle glfw vulkan)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
