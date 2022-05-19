cmake_minimum_required(VERSION 3.20)
project(my_cpp_library)

set(CMAKE_CXX_STANDARD 14)

add_executable(my_cpp_library graph.h ThreadPool/ThreadPool.cpp ThreadPool/ThreadPool.h ThreadPool/Blocking_deque.cpp ThreadPool/Blocking_deque.h ThreadPool/test.h main.cpp Louvain/Louvain.cpp Louvain/Louvain.h)

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

