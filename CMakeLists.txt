cmake_minimum_required(VERSION 3.5.0)

project(CubeCode)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release) #Debug/Release
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/)
option(TEST_BUILD "Excute Test Source Build" ON)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/SoftwareConfig.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/SoftwareConfig.h
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/Utils)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/SystemV)
if(${TEST_BUILD})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test/SystemV)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test/POSIX)
endif()

################################################## Include ##############################################

set(INC_DIRECTORY_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SystemV
    ${CMAKE_CURRENT_SOURCE_DIR}/src/POSIX
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/zlib-1.2.11
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/rapidjson
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/rapidjson/error
)

foreach(inc_directory ${INC_DIRECTORY_ALL})
include_directories(${inc_directory})
endforeach()

################################################## Source ##############################################
set(SRC_DIRECTORY_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Common
)

foreach(src_directory ${SRC_DIRECTORY_ALL})
aux_source_directory(${src_directory} SRC_DIRECTORY_LIST)
endforeach()

########################################################################################################

add_executable(${PROJECT_NAME} ${SRC_DIRECTORY_LIST})

target_link_libraries(
    ${PROJECT_NAME} 
    pthread
    SystemV
    Utils
)
