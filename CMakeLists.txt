cmake_minimum_required(VERSION 3.15)

project(endpoint)

include(common.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR})

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    set(LIBRARIES Bthprops Ws2_32 Iphlpapi)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Gio REQUIRED gio-2.0 gio-unix-2.0 IMPORTED_TARGET)
    set(LIBRARIES PkgConfig::Gio)
endif()

include_directories(thirdparty endpoint)

aux_source_directory(thirdparty/base64 SOURCES)
aux_source_directory(thirdparty/sha1 SOURCES)
aux_source_directory(endpoint SOURCES)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

add_executable(server sample/server.cpp)
target_link_libraries(server PRIVATE ${PROJECT_NAME})

add_executable(client sample/client.cpp)
target_link_libraries(client PRIVATE ${PROJECT_NAME})
