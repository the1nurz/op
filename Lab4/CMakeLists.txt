cmake_minimum_required(VERSION 3.20)
project(Lab4 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Lab4 requires Windows and the WinAPI/GDI")
endif()
enable_language(RC)

configure_file(resources/Lab4.rc.in "${CMAKE_CURRENT_BINARY_DIR}/Lab4Resources.rc" @ONLY)

set(LAB4_MODEL_SOURCES
    src/Shape.cpp
    src/PointShape.cpp
    src/LineShape.cpp
    src/RectShape.cpp
    src/EllipseShape.cpp
    src/CircleLineShape.cpp
    src/CubeShape.cpp
)
set(LAB4_APP_SOURCES
    ${LAB4_MODEL_SOURCES}
    src/MyEditor.cpp
    src/MainWindow.cpp
)

add_executable(Lab4 WIN32
    src/main.cpp
    ${LAB4_APP_SOURCES}
    "${CMAKE_CURRENT_BINARY_DIR}/Lab4Resources.rc"
)
target_include_directories(Lab4 PRIVATE resources)
target_compile_features(Lab4 PRIVATE cxx_std_17)
target_compile_definitions(Lab4 PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(Lab4 PRIVATE user32 gdi32 comctl32)

if(MSVC)
    target_compile_options(Lab4 PRIVATE /W4 /utf-8)
else()
    target_compile_options(Lab4 PRIVATE -Wall -Wextra -Wpedantic)
    target_link_options(Lab4 PRIVATE -municode -static -static-libgcc -static-libstdc++)
endif()

option(LAB4_BUILD_TESTS "Build model and editor smoke tests" OFF)
if(LAB4_BUILD_TESTS)
    enable_testing()
    add_executable(Lab4ModelSmoke tests/model_smoke.cpp ${LAB4_MODEL_SOURCES})
    add_executable(Lab4EditorSmoke tests/editor_smoke.cpp src/MyEditor.cpp ${LAB4_MODEL_SOURCES})
    add_executable(Lab4WindowSmoke tests/window_smoke.cpp ${LAB4_APP_SOURCES}
        "${CMAKE_CURRENT_BINARY_DIR}/Lab4Resources.rc")
    foreach(target Lab4ModelSmoke Lab4EditorSmoke Lab4WindowSmoke)
        target_include_directories(${target} PRIVATE src)
        target_compile_features(${target} PRIVATE cxx_std_17)
        target_compile_definitions(${target} PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
        target_link_libraries(${target} PRIVATE user32 gdi32)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4 /utf-8 /UNDEBUG)
        else()
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
            target_link_options(${target} PRIVATE -static -static-libgcc -static-libstdc++)
        endif()
    endforeach()
    target_include_directories(Lab4WindowSmoke PRIVATE resources)
    target_link_libraries(Lab4WindowSmoke PRIVATE comctl32)
    add_test(NAME Lab4ModelSmoke COMMAND Lab4ModelSmoke)
    add_test(NAME Lab4EditorSmoke COMMAND Lab4EditorSmoke)
    add_test(NAME Lab4WindowSmoke COMMAND Lab4WindowSmoke)
endif()
