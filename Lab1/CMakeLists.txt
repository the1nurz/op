cmake_minimum_required(VERSION 3.20)
project(Lab1 LANGUAGES CXX RC)

if(NOT WIN32)
    message(FATAL_ERROR "Lab1 requires Windows and the WinAPI.")
endif()

option(LAB1_BUILD_TESTS "Build WinAPI integration smoke checks" OFF)
if(LAB1_BUILD_TESTS)
    add_executable(Lab1Smoke tests/smoke.cpp module1.cpp module2.cpp resource.rc module1.rc module2.rc)
    target_compile_features(Lab1Smoke PRIVATE cxx_std_17)
    target_compile_definitions(Lab1Smoke PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(Lab1Smoke PRIVATE user32 gdi32)
    if(MSVC)
        target_compile_options(Lab1Smoke PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/W4> $<$<COMPILE_LANGUAGE:CXX>:/utf-8>)
    elseif(MINGW)
        target_compile_options(Lab1Smoke PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic;-finput-charset=UTF-8>")
        target_link_options(Lab1Smoke PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif()

add_executable(Lab1 WIN32
    main.cpp
    module1.cpp module1.h module1_resources.h module1.rc
    module2.cpp module2.h module2_resources.h module2.rc
    dialog_result.h resource.h resource.rc
)
target_compile_features(Lab1 PRIVATE cxx_std_17)
target_compile_definitions(Lab1 PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(Lab1 PRIVATE user32 gdi32)

if(MSVC)
    target_compile_options(Lab1 PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/W4> $<$<COMPILE_LANGUAGE:CXX>:/utf-8>)
else()
    target_compile_options(Lab1 PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic;-finput-charset=UTF-8>")
    if(MINGW)
        target_link_options(Lab1 PRIVATE -municode -static-libgcc -static-libstdc++)
    endif()
endif()
