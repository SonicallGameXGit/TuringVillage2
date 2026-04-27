cmake_minimum_required(VERSION 3.10)
project(TuringVillage2 VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/SDL)
add_subdirectory(libs/SDL_image)
add_subdirectory(libs/GLAD)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/gl/shader.cpp
    src/gl/buffer.cpp
    src/gl/texture.cpp
    src/villager.cpp
    src/cursor.cpp
    src/window.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image GLAD)
target_include_directories(${PROJECT_NAME} PRIVATE
    libs/SDL/include
    libs/SDL_image/include
    libs/GLAD/include
)