cmake_minimum_required(VERSION 3.15)
project(CLion_challenges4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your executable (only .cpp files!)
add_executable(CLion_challenges4
        main_challenge1.cpp
)

# Link Windows Multimedia library for PlaySound
if(WIN32)
    target_link_libraries(CLion_challenges4 PRIVATE winmm)
endif()
