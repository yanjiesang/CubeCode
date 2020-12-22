
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
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Common
)

foreach(src_directory ${SRC_DIRECTORY_ALL})
aux_source_directory(${src_directory} SRC_DIRECTORY_LIST)
endforeach()

########################################################################################################

add_executable(
    "MmapStream"
    ${SRC_DIRECTORY_LIST}
)

target_link_libraries(
    "MmapStream"
    pthread
    SystemV
    Utils
)



