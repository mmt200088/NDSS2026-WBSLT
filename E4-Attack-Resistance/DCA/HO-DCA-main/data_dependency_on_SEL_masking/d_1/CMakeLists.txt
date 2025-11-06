cmake_minimum_required(VERSION 2.8)

project(WBAES)

include_directories(include)

aux_source_directory(src WB_SRC)

add_library(WB_LIB ${WB_SRC})

add_executable(WBAES test/main.c)

# Link WB_LIB and the math library (-lm)
target_link_libraries(WBAES WB_LIB m)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2")
