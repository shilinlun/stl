cmake_minimum_required(VERSION 3.17)
project(stl)

set(CMAKE_CXX_STANDARD 14)

add_executable(stl main.cpp header_files/algobase.h header_files/allocator.h header_files/construct.h header_files/iterator.h header_files/type_traits.h header_files/uninitialized.h header_files/utils.h header_files/vector.h header_files/memory.h header_files/algorithm.h header_files/list.h header_files/deque.h header_files/stack.h header_files/queue.h header_files/heap_algo.h)