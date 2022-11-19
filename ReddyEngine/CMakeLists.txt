project(ReddyEngine)
set(CMAKE_CXX_STANDARD 17)

# Engine files
file(GLOB reddyengine_src src/*.*)
list(APPEND reddyengine_includes PRIVATE src)
list(APPEND reddyengine_includes PUBLIC include)

# SDL
add_definitions(-DSDL_MAIN_HANDLED)
set(HAVE_LIBC ON)
add_subdirectory(SDL2-Premake)
list(APPEND reddyengine_libs SDL2-static)
list(APPEND reddyengine_includes PUBLIC SDL2-Premake/include)

# lua
add_subdirectory(lua)
list(APPEND reddyengine_includes PUBLIC lua)

# JsonCPP
list(APPEND reddyengine_src 
    jsoncpp/src/lib_json/json_reader.cpp
    jsoncpp/src/lib_json/json_value.cpp
    jsoncpp/src/lib_json/json_writer.cpp
)
list(APPEND reddyengine_includes PUBLIC jsoncpp/include)

# glm
list(APPEND reddyengine_includes PUBLIC glm)

# stb
list(APPEND reddyengine_includes PUBLIC stb-premake)

# imgui
list(APPEND reddyengine_src 
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_sdl.cpp
)
list(APPEND reddyengine_includes PUBLIC imgui)

# Tinyfiledialogs
list(APPEND reddyengine_src tinyfiledialogs/tinyfiledialogs.c)

add_library(${PROJECT_NAME} STATIC ${reddyengine_src})

target_include_directories(${PROJECT_NAME} ${reddyengine_includes})
target_link_libraries(${PROJECT_NAME} PUBLIC ${reddyengine_libs})
