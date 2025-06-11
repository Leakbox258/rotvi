cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)

project(rotvi)

find_package(Curses REQUIRED)

# Use gdb only
add_compile_options("-g")
add_link_options("-rdynamic")

# Enable extended source for ncurses to support extended functions
add_definitions(-D_XOPEN_SOURCE_EXTENDED)

aux_source_directory(lib LIB)

add_executable(rotvi ${LIB})

target_include_directories(rotvi 
    PRIVATE ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/include/syntax
    PRIVATE ${CMAKE_SOURCE_DIR}/include/syntax/CXX
    PRIVATE ${CURSES_INCLUDE_DIR}
)

# Link against ncursesw for wide character and extended color support
target_link_libraries(rotvi PRIVATE ncursesw)