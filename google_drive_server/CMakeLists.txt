cmake_minimum_required(VERSION 3.16.3)

project(Google_Drive_Project)

if (DEFINED boost_custom)
    set(BOOST_INCLUDEDIR "${boost_custom}")
    set(BOOST_LIBRARYDIR "${boost_custom}/stage/lib")
    set(BOOST_ROOT "${boost_custom}/boost")
endif ()

if (DEFINED chilkat)
    set(CHILKAT_DIR ${chilkat})
    set(CHILKAT_INC ${chilkat}/include)
    if (DEFINED WIN32)
        set(CHILKAT_LIB ${chilkat})
    else ()
        set(CHILKAT_LIB ${chilkat}/lib)
    endif ()
else ()
    set(CHILKAT_DIR chilkat-9.5.0)
    set(CHILKAT_INC chilkat-9.5.0/include)
    if (DEFINED WIN32)
        set(CHILKAT_LIB chilkat-9.5.0)
    else ()
        set(CHILKAT_LIB chilkat-9.5.0/lib)
    endif ()
endif ()

find_package(Boost 1.74.0 )

link_directories( )

include_directories(
    inc
    ${CHILKAT_INC}
    ${Boost_INCLUDE_DIRS}
    )

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${CHILKAT_LIB})

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


if (DEFINED WIN32)
    target_link_libraries(google_project libchilkat-9.5.0.a crypt32 ws2_32 dnsapi)
else ()
    target_link_libraries(google_project libchilkat-9.5.0.a pthread)
endif ()

