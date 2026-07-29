cmake_minimum_required(VERSION 3.24)
project(Monolith VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.5
    GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW ON
    )
FetchContent_MakeAvailable(glm)

find_package(OpenGL REQUIRED)

add_library(MonolithCore SHARED
    engine/src/App.cpp
    engine/src/input.cpp
    engine/src/Camera2D.cpp
)

target_include_directories(MonolithCore PUBLIC engine/include)
target_include_directories(MonolithCore SYSTEM PUBLIC ${sdl2_SOURCE_DIR}/include)

target_link_libraries(MonolithCore PRIVATE SDL2::SDL2-static SDL2::SDL2main OpenGL::GL)
target_link_libraries(MonolithCore PUBLIC glm::glm)

add_executable(MonolithSandbox 
    sandbox/src/main.cpp 
)

target_link_libraries(MonolithSandbox PRIVATE MonolithCore)

