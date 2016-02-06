project(QSqliteProfiler)

cmake_minimum_required(VERSION 2.8)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x") 
endif()

find_package(Qt5Core)
find_package(Qt5Sql)

set(CMAKE_AUTOMOC ON)

include_directories(${Qt5Core_INCLUDE_DIRS}
                    ${Qt5Sql_INCLUDE_DIRS}
                    src
)

add_subdirectory(src)
add_subdirectory(test)