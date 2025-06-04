cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)

project(rotvi)

option(ENABLE_ASAN "Enable AddressSanitizer" ON)

find_package(Curses REQUIRED)

# use gdb only
add_compile_options("-g")
add_link_options("-rdynamic")

if (ENABLE_ASAN)
        message(STATUS "AddressSanitizer enabled")
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
endif()

aux_source_directory(lib LIB)

add_executable(rotvi ${LIB})

target_include_directories(rotvi 
    PRIVATE ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/include/syntax
    PRIVATE ${CMAKE_SOURCE_DIR}/include/syntax/CXX
    PRIVATE ${CURSES_INCLUDE_DIR}
)

target_link_libraries(rotvi PRIVATE ${CURSES_LIBRARIES})