cmake_minimum_required(VERSION 3.18.3)

project(Google_Drive_Project)

set(BOOST_INCLUDEDIR "D:/all_with_prog/plugins/boost_1_74_0")
set(BOOST_LIBRARYDIR "D:/all_with_prog/plugins/boost_1_74_0/stage/lib")
set(BOOST_ROOT "D:/all_with_prog/plugins/boost_1_70_0/boost")

find_package(Boost 1.74.0)

link_directories( )

include_directories(
    inc
    chilkat-9.5.0/include
    ${Boost_INCLUDE_DIRS}
    )

link_directories(
    ${Boost_LIBRARY_DIRS}
    chilkat-9.5.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")


set(SRC
    src/main.cpp
    src/GoogleDriveController.cpp
    src/ConnectionController.cpp
    src/Request.cpp
    src/TaskController.cpp
    src/Task.cpp
    src/Responce.cpp
    src/UserController.cpp
    src/User.cpp
)

add_executable(google_project ${SRC})

target_link_libraries(google_project libchilkat-9.5.0.a crypt32 ws2_32 dnsapi)
