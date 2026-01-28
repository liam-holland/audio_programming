cmake_minimum_required(VERSION 3.15)
project(CLion_challenges4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(challenge1 main_challenge1.cpp)
add_executable(challenge2 main_challenge2.cpp)
add_executable(challenge3 main_challenge3.cpp)

if (WIN32)
    target_link_libraries(challenge1 PRIVATE winmm)
    target_link_libraries(challenge2 PRIVATE winmm)
    target_link_libraries(challenge3 PRIVATE winmm)
endif()
