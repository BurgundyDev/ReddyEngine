cmake_minimum_required(VERSION 3.0)

project(ReddyGame)
set(CMAKE_CXX_STANDARD 17)

# Game files
list(APPEND reddygame_includes PUBLIC 
    ReddyGame/src 
    ReddyEngine/include
    ReddyEngine/glm
    ReddyEngine/jsoncpp/include
    ReddyEngine/SDL2-Premake/include
    ReddyEngine/imgui
)
file(GLOB reddygame_src ReddyGame/src/*.*)

# ReddyEngine
add_subdirectory(ReddyEngine)

# executable
add_executable(${PROJECT_NAME} ${reddygame_src})

target_include_directories(${PROJECT_NAME} PUBLIC ${reddygame_includes})
