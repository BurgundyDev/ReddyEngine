project(lua)

file(GLOB lua_src *.c)
list(REMOVE_ITEM lua_src luac.c)
list(APPEND lua_includes PUBLIC ./)

add_library(${PROJECT_NAME} STATIC ${lua_src})

target_include_directories(${PROJECT_NAME} ${lua_includes})
