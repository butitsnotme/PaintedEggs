cmake_minimum_required(VERSION 3.0)

project(painted_eggs)

set(BINARY "PaintedEggs")
list(APPEND SOURCES "main.cpp")


set(THREADS_PREFER_PTHREAD_FLAD ON)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)


set(LIBS Threads::Threads OpenGL::GL)


if (UNIX)
    find_package(X11 REQUIRED)
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    list(APPEND LIBS ${X11_LIBRARIES})
    list(APPEND LIBS PNG::PNG)
    list(APPEND LIBS OpenGL::OpenGL)
    list(APPEND LIBS OpenGL::GLX)
endif (UNIX)
if (WIN32)
    include_directories(${WinSDK})
    list(APPEND LIBS ${WindSDK})
endif (WIN32)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${BINARY} ${SOURCES})

add_custom_command(
  TARGET ${BINARY} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/layers
    ${CMAKE_BINARY_DIR}/layers)

add_custom_command(
  TARGET ${BINARY} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/egg.png
    ${CMAKE_BINARY_DIR})

add_custom_command(
  TARGET ${BINARY} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/guy.png
    ${CMAKE_BINARY_DIR})

install(TARGETS ${BINARY} RUNTIME DESTINATION bin)
target_link_libraries(${BINARY} ${LIBS})
