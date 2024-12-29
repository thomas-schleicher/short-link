cmake_minimum_required(VERSION 3.16)

project(short-link VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILD_SHARED_LIBS OFF)

include_directories(${PROJECT_SOURCE_DIR}/includes)

add_executable(Application
        src/main.cpp
        src/http_connection.cpp
        src/http_server.cpp
        src/request_handler.cpp
        src/database_connection.cpp
)

add_custom_target(copy_frontend ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/frontend ${CMAKE_BINARY_DIR}/frontend
)

find_package(Boost REQUIRED COMPONENTS filesystem system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries(Application PRIVATE Boost::filesystem Boost::system)
else()
    message(FATAL_ERROR "Boost not found!")
endif()

find_package(SOCI REQUIRED sqlite3)

if(SOCI_FOUND)
    include_directories(${SOCI_INCLUDE_DIRS})
    target_link_libraries(Application PRIVATE soci_core soci_sqlite3)
else()
    message(FATAL_ERROR "SOCI not found!")
endif()

find_package(SQLite3 REQUIRED)

if(SQLite3_FOUND)
    include_directories(${SQLite3_INCLUDE_DIRS})
    target_link_libraries(Application PRIVATE SQLite::SQLite3)
else()
    message(FATAL_ERROR "SQLite3 not found!")
endif()
