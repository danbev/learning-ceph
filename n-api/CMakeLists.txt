cmake_minimum_required(VERSION 2.8)

project(cephnapi)

find_library(RADOS_LIBRARY rados REQUIRED)

file(GLOB SOURCE_FILES "src/*.cc" "src/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api
    PRIVATE /usr/include/rados
    PRIVATE ${CMAKE_JS_INC})

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${RADOS_LIBRARY})
