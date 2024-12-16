cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0167 OLD)

project(short-link VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILD_SHARED_LIBS OFF)

include_directories(${PROJECT_SOURCE_DIR}/includes)
add_executable(Application src/main.cpp
    src/http_connection.cpp
    src/http_server.cpp
    src/request_handler.cpp
)

find_package(Boost REQUIRED filesystem system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries(Application PRIVATE Boost::filesystem Boost::system)
else()
    message(FATAL_ERROR "Boost not found!")
endif()