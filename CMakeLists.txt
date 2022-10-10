cmake_minimum_required(VERSION 3.0.0)
project(fuerza-bruta)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

find_program(LEX_EXE
    flex
)
if(LEX_EXE STREQUAL "LEX_EXE-NOTFOUND")
    message(FATAL_ERROR "flex executable not found")
endif(LEX_EXE STREQUAL "LEX_EXE-NOTFOUND")

# lex source file macro
MACRO(LEX_FILE _filename)
    GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
    ADD_CUSTOM_COMMAND(
        OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.c
        COMMAND ${LEX_EXE}
                -o${_basename}.c
                -L
                ${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
        DEPENDS ${_filename} )
ENDMACRO(LEX_FILE)

LEX_FILE(src/grammar.lex)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/grammar.c GENERATED)

add_executable(fuerza-bruta src/main.c grammar.c)
target_include_directories(fuerza-bruta PUBLIC src)
