cmake_minimum_required(VERSION 3.20)
project(thread_test)

set(CMAKE_CXX_STANDARD 14)

add_executable(thread_test main.cpp ThreadPool.cpp ThreadPool.h Blocking_deque.cpp Blocking_deque.h)

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

