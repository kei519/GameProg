# GameLib
set(LIB_NAME GameLib)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    file(GLOB_RECURSE SRCS Windows/*.cpp)
    list(APPEND PRIVATE_INCLUDE_DIRS Windows/Modules/src)
    list(APPEND PRIVATE_LIBRARIES
        winmm.lib
        d3d9.lib
        dinput8.lib
        dxguid.lib
        dsound.lib
    )
    list(APPEND PUBLIC_INCLUDE_DIRS Windows)
else()
    message(SEND_ERROR "not supported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_library(${LIB_NAME} STATIC ${SRCS})
list(APPEND PUBLIC_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)

target_include_directories(${LIB_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS})
target_include_directories(${LIB_NAME} PRIVATE ${PRIVATE_INCLUDE_DIRS})

target_link_libraries(${LIB_NAME} PUBLIC ${PUBLIC_LIBRARIES})
target_link_libraries(${LIB_NAME} PRIVATE ${PRIVATE_LIBRARIES})

# Util
set(LIB_NAME Util)
set(PUBLIC_INCLUDE_DIRS)
set(PRIVATE_INCLUDE_DIRS)
set(PUBLIC_LIBRARIES)
set(PRIVATE_LIBRARIES)

file(GLOB_RECURSE SRCS Util/*.cpp)

add_library(${LIB_NAME} STATIC ${SRCS})
list(APPEND PUBLIC_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)
list(APPEND PRIVATE_LIBRARIES GameLib)

target_include_directories(${LIB_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS})
target_include_directories(${LIB_NAME} PRIVATE ${PRIVATE_INCLUDE_DIRS})

target_link_libraries(${LIB_NAME} PUBLIC ${PUBLIC_LIBRARIES})
target_link_libraries(${LIB_NAME} PRIVATE ${PRIVATE_LIBRARIES})
