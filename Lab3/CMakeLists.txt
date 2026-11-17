cmake_minimum_required(VERSION 3.20)
project(Lab3 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Lab3 requires Windows and the WinAPI/GDI")
endif()
enable_language(RC)

configure_file(resources/Lab3.rc.in "${CMAKE_CURRENT_BINARY_DIR}/Lab3Resources.rc" @ONLY)

set(LAB3_MODEL_SOURCES
    src/Shape.cpp
    src/PointShape.cpp
    src/LineShape.cpp
    src/RectShape.cpp
    src/EllipseShape.cpp
)
set(LAB3_APP_SOURCES
    ${LAB3_MODEL_SOURCES}
    src/MyEditor.cpp
    src/MainWindow.cpp
)

add_executable(Lab3 WIN32
    src/main.cpp
    ${LAB3_APP_SOURCES}
    "${CMAKE_CURRENT_BINARY_DIR}/Lab3Resources.rc"
)
target_include_directories(Lab3 PRIVATE resources)
target_compile_features(Lab3 PRIVATE cxx_std_17)
target_compile_definitions(Lab3 PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(Lab3 PRIVATE user32 gdi32 comctl32)

if(MSVC)
    target_compile_options(Lab3 PRIVATE /W4 /utf-8)
else()
    target_compile_options(Lab3 PRIVATE -Wall -Wextra -Wpedantic)
    target_link_options(Lab3 PRIVATE -municode -static -static-libgcc -static-libstdc++)
endif()

option(LAB3_BUILD_TESTS "Build model and editor smoke tests" OFF)
if(LAB3_BUILD_TESTS)
    enable_testing()
    add_executable(Lab3ModelSmoke tests/model_smoke.cpp ${LAB3_MODEL_SOURCES})
    add_executable(Lab3EditorSmoke tests/editor_smoke.cpp src/MyEditor.cpp ${LAB3_MODEL_SOURCES})
    foreach(target Lab3ModelSmoke Lab3EditorSmoke)
        target_compile_features(${target} PRIVATE cxx_std_17)
        target_compile_definitions(${target} PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
        target_link_libraries(${target} PRIVATE user32 gdi32)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4 /utf-8)
        else()
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        endif()
    endforeach()
    add_test(NAME Lab3ModelSmoke COMMAND Lab3ModelSmoke)
    add_test(NAME Lab3EditorSmoke COMMAND Lab3EditorSmoke)
endif()
