#Common options
cmake_minimum_required(VERSION 3.21)
project(rv32esim VERSION 0.1)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#-I is not needed to compile but is needed for clangd to find the include directory
#set(CMAKE_C_FLAGS "-I include -Wall -Wextra -g3 -mavx -mavx2 -mavx512f")
#set(CMAKE_C_FLAGS "-I include -Wall -Wextra -g3 -mavx -mavx2 -mfma")
#set(CMAKE_C_FLAGS "-I include -Wall -Wextra -g3 -mavx -mavx2)
#set(CMAKE_C_FLAGS "-I include -Wall -Wextra -g3 -mavx")
set(CMAKE_C_FLAGS "-I include -Wall -Wextra -g3")
#set(CMAKE_C_FLAGS "-I include -Wall -Wextra -g3 -mno-sse2")
set(CMAKE_C_FLAGS_DEBUG "-O0")
set(CMAKE_C_FLAGS_RELEASE "-Ofast -flto=auto -fuse-linker-plugin -floop-nest-optimize -fipa-pta -fno-semantic-interposition -fdevirtualize-at-ltrans -fno-plt -fstdarg-opt -frename-registers -fweb -ftree-vectorize")

#rv32esim Library Targets
set(RV32ESIM_SOURCES src/rv32esim/rv32esim.c src/rv32esim/common.h src/rv32esim/fetch.c src/rv32esim/fetch.h src/rv32esim/decode.c src/rv32esim/decode.h src/rv32esim/execute.c src/rv32esim/execute.h src/rv32esim/logging.c src/rv32esim/logging.h src/rv32esim/cmake_config.h.in include/rv32esim.h)

if (CMAKE_C_BYTE_ORDER STREQUAL LITTLE_ENDIAN)
    set(IS_LITTLE_ENDIAN 1)
else()
    set(IS_LITTLE_ENDIAN 0)
endif()

configure_file(src/rv32esim/cmake_config.h.in src/rv32esim/cmake_config.h)

add_library(rv32esim_shared SHARED ${RV32ESIM_SOURCES})
add_library(rv32esim_static STATIC ${RV32ESIM_SOURCES})
target_include_directories(rv32esim_shared PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/src/rv32esim/)
target_include_directories(rv32esim_static PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/src/rv32esim/)
set_property(TARGET rv32esim_shared PROPERTY OUTPUT_NAME rv32esim)
set_property(TARGET rv32esim_static PROPERTY OUTPUT_NAME rv32esim)

#rv32esim Test Code
set(TEST_SOURCES src/main.c include/rv32esim.h)
add_executable(rv32esim ${TEST_SOURCES})
target_include_directories(rv32esim PRIVATE include)
target_link_libraries(rv32esim rv32esim_shared)

#These don't work at stopping hidden symbols from being exported from the static library
#target_link_libraries(rv32esim rv32esim_static)
#set_target_properties(rv32esim PROPERTIES C_VISIBILITY_PRESET hidden)
#target_link_options(rv32esim PRIVATE -Wl,--exclude-libs=ALL)
