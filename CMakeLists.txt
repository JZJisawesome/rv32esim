cmake_minimum_required(VERSION 3.21)

project(rvsim32ie VERSION 0.1)

set(SOURCES src/main.c)
#set(SOURCES src/main.c src/rvsim/rvsim.c src/rvsim/memory.c src/rvsim/memory.h src/rvsim/fetch.c src/rvsim/fetch.h src/rvsim/endianness.h src/rvsim/decode.c src/rvsim/decode.h src/rvsim/execute.c src/rvsim/execute.h src/rvsim/debug_logging.c src/rvsim/debug_logging.h include/rvsim.h)

add_executable(rvsim32ie ${SOURCES})

#target_include_directories(rvsim32ie PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include)


#set_property(TARGET rvsim PROPERTY C_STANDARD 11)

#target_link_libraries(mbbmp m pthread)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#set(CMAKE_C_FLAGS "-Wall -Wextra -g3 -mavx -mavx2 -mavx512f")
#set(CMAKE_C_FLAGS "-Wall -Wextra -g3 -mavx -mavx2 -mfma")
#set(CMAKE_C_FLAGS "-Wall -Wextra -g3 -mavx -mavx2)
#set(CMAKE_C_FLAGS "-Wall -Wextra -g3 -mavx")
set(CMAKE_C_FLAGS "-Wall -Wextra -g3")
#set(CMAKE_C_FLAGS "-Wall -Wextra -g3 -mno-sse2")
set(CMAKE_C_FLAGS_DEBUG "-O0")
set(CMAKE_C_FLAGS_RELEASE "-Ofast -flto=auto -fuse-linker-plugin -floop-nest-optimize -fipa-pta -fno-semantic-interposition -fdevirtualize-at-ltrans -fno-plt -fstdarg-opt -frename-registers -fweb -ftree-vectorize")


#if (CMAKE_C_BYTE_ORDER STREQUAL LITTLE_ENDIAN)
#    set(MBBMP_LITTLE_ENDIAN 1)
#else()
#    set(MBBMP_LITTLE_ENDIAN 0)
#endif()

#configure_file(include/cmake_config.h.in include/cmake_config.h)
