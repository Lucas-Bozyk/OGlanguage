cmake_minimum_required(VERSION 3.20)
project(OGLanguageAutomatos VERSION 0.1.0 LANGUAGES CXX)

add_library(ogl_automatos
    src/afn.cpp
    src/afd.cpp
    src/thompson.cpp
    src/determinizacao.cpp
    src/minimizacao.cpp
)
target_include_directories(ogl_automatos PUBLIC include)
target_compile_features(ogl_automatos PUBLIC cxx_std_20)
set_target_properties(ogl_automatos PROPERTIES CXX_EXTENSIONS OFF)

function(ogl_avisos alvo)
    if(MSVC)
        target_compile_options(${alvo} PRIVATE /W4 /WX /permissive- /utf-8)
    else()
        target_compile_options(${alvo} PRIVATE -Wall -Wextra -Werror -pedantic)
    endif()
endfunction()

ogl_avisos(ogl_automatos)
add_executable(ogl main.cpp)
target_link_libraries(ogl PRIVATE ogl_automatos)
ogl_avisos(ogl)

include(CTest)
if(BUILD_TESTING)
    foreach(modulo IN ITEMS afn afd thompson determinizacao minimizacao)
        add_executable(test_${modulo} tests/test_${modulo}.cpp)
        target_link_libraries(test_${modulo} PRIVATE ogl_automatos)
        ogl_avisos(test_${modulo})
        add_test(NAME ${modulo} COMMAND test_${modulo})
    endforeach()
    add_test(NAME interface
        COMMAND ${CMAKE_COMMAND}
            -DOGL_EXECUTAVEL=$<TARGET_FILE:ogl>
            -DOGL_EXEMPLOS=${CMAKE_CURRENT_SOURCE_DIR}/examples
            -DOGL_TEMPORARIOS=${CMAKE_CURRENT_BINARY_DIR}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_interface.cmake
    )
endif()
