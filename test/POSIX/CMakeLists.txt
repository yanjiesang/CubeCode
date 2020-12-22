
################################################## Include ##############################################

set(INC_DIRECTORY_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/SystemV
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/POSIX
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Utils/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Utils/zlib-1.2.11
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Utils/rapidjson
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Utils/rapidjson/error
)

foreach(inc_directory ${INC_DIRECTORY_ALL})
include_directories(${inc_directory})
endforeach()

################################################## Source ##############################################
set(SRC_DIRECTORY_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Common
)

foreach(src_directory ${SRC_DIRECTORY_ALL})
aux_source_directory(${src_directory} SRC_DIRECTORY_LIST)
endforeach()

########################################################################################################

file(
GLOB
POSIX_READ_SOURCE
${CMAKE_CURRENT_SOURCE_DIR}/PosixMmapRead.cpp
)

file(
GLOB
POSIX_WRITE_SOURCE
${CMAKE_CURRENT_SOURCE_DIR}/PosixMmapWrite.cpp
)

add_executable(
    "PosixMmapRead"
    ${POSIX_READ_SOURCE}
    ${SRC_DIRECTORY_LIST}
)

target_link_libraries(
    "PosixMmapRead"
    pthread
    SystemV
    Posix
    Utils
)

add_executable(
    "PosixMmapWrite"
    ${POSIX_WRITE_SOURCE}
    ${SRC_DIRECTORY_LIST}
)

target_link_libraries(
    "PosixMmapWrite"
    pthread
    SystemV
    Posix
    Utils
)


