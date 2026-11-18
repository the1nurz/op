cmake_minimum_required(VERSION 3.20)
project(Lab5 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Lab5 requires Windows and the WinAPI/GDI")
endif()
enable_language(RC)

configure_file(resources/Lab5.rc.in "${CMAKE_CURRENT_BINARY_DIR}/Lab5Resources.rc" @ONLY)

set(LAB5_MODEL_SOURCES
    src/Shape.cpp
    src/PointShape.cpp
    src/LineShape.cpp
    src/RectShape.cpp
    src/EllipseShape.cpp
    src/CircleLineShape.cpp
    src/ShapeFactory.cpp
    src/CubeShape.cpp
)
set(LAB5_APP_SOURCES
    ${LAB5_MODEL_SOURCES}
    src/MyEditor.cpp
    src/MyTable.cpp
    src/MainWindow.cpp
)

add_executable(Lab5 WIN32
    src/main.cpp
    ${LAB5_APP_SOURCES}
    "${CMAKE_CURRENT_BINARY_DIR}/Lab5Resources.rc"
)
target_include_directories(Lab5 PRIVATE resources)
target_compile_features(Lab5 PRIVATE cxx_std_17)
target_compile_definitions(Lab5 PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(Lab5 PRIVATE user32 gdi32 comctl32 comdlg32)

if(MSVC)
    target_compile_options(Lab5 PRIVATE /W4 /utf-8)
else()
    target_compile_options(Lab5 PRIVATE -Wall -Wextra -Wpedantic)
    target_link_options(Lab5 PRIVATE -municode -static -static-libgcc -static-libstdc++)
endif()

option(LAB5_BUILD_TESTS "Build model and editor smoke tests" OFF)
if(LAB5_BUILD_TESTS)
    enable_testing()
    add_executable(Lab5ModelSmoke tests/model_smoke.cpp ${LAB5_MODEL_SOURCES})
    add_executable(Lab5EditorSmoke tests/editor_smoke.cpp src/MyEditor.cpp ${LAB5_MODEL_SOURCES})
    add_executable(Lab5WindowSmoke tests/window_smoke.cpp ${LAB5_APP_SOURCES}
        "${CMAKE_CURRENT_BINARY_DIR}/Lab5Resources.rc")
    foreach(target Lab5ModelSmoke Lab5EditorSmoke Lab5WindowSmoke)
        target_include_directories(${target} PRIVATE src)
        target_compile_features(${target} PRIVATE cxx_std_17)
        target_compile_definitions(${target} PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
        target_link_libraries(${target} PRIVATE user32 gdi32 comdlg32)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4 /utf-8 /UNDEBUG)
        else()
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
            target_link_options(${target} PRIVATE -static -static-libgcc -static-libstdc++)
        endif()
    endforeach()
    target_include_directories(Lab5WindowSmoke PRIVATE resources)
    target_link_libraries(Lab5WindowSmoke PRIVATE comctl32)
    add_test(NAME Lab5ModelSmoke COMMAND Lab5ModelSmoke)
    add_test(NAME Lab5EditorSmoke COMMAND Lab5EditorSmoke)
    add_test(NAME Lab5WindowSmoke COMMAND Lab5WindowSmoke)
endif()
