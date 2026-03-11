cmake_minimum_required(VERSION 3.25)

project(U1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
)

add_executable(U1 ${SOURCE_FILES})

find_package(Boost REQUIRED)
target_include_directories(U1 PUBLIC ${Boost_INCLUDE_DIRS})
if(WIN32)
    target_link_libraries(U1 
        PRIVATE
            Bcrypt crypt32)
endif()
