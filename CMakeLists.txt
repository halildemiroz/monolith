cmake_minimum_required(VERSION 3.24)
project(Monolith VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
set(SDL_PIPEWIRE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW ON
    )
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    GIT_SHALLOW ON
    )
FetchContent_MakeAvailable(imgui)

FetchContent_declare(
    stb_image
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW ON
    )
FetchContent_MakeAvailable(stb_image)

add_library(ImGui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(ImGui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${sdl2_SOURCE_DIR}/include
    engine/vendor/glad/include
)

target_compile_definitions(ImGui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(ImGui PUBLIC SDL2::SDL2-static)

find_package(OpenGL REQUIRED)

add_library(MonolithCore SHARED
    engine/src/App.cpp
    engine/src/Shader.cpp
    engine/src/Input.cpp
    engine/src/Renderer.cpp
    engine/src/Camera2D.cpp
    engine/src/VAO.cpp
    engine/src/VBO.cpp
    engine/src/Texture.cpp
    engine/src/TextureLib.cpp
    engine/vendor/glad/src/glad.c
)

target_include_directories(MonolithCore PUBLIC engine/include)
target_include_directories(MonolithCore SYSTEM PUBLIC ${sdl2_SOURCE_DIR}/include)
target_include_directories(MonolithCore PUBLIC ${stb_image_SOURCE_DIR})
target_include_directories(MonolithCore PUBLIC engine/vendor/glad/include)

target_link_libraries(MonolithCore PRIVATE SDL2::SDL2-static SDL2::SDL2main OpenGL::GL)
target_link_libraries(MonolithCore PUBLIC glm::glm ImGui)

add_executable(MonolithSandbox 
    sandbox/src/main.cpp 
    sandbox/src/sandboxLayer.cpp
)

target_include_directories(MonolithSandbox PUBLIC sandbox/include)
target_link_libraries(MonolithSandbox PRIVATE MonolithCore)

