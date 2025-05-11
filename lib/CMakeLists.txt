set(LIB_NAME GameLib)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    file(GLOB_RECURSE SRCS Windows/*.cpp)
    list(APPEND PRIVATE_INCLUDE_DIRS Windows/Modules/src)
    list(APPEND PRIVATE_LIBRARIES winmm.lib)
else()
    message(SEND_ERROR "not supported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_library(${LIB_NAME} STATIC ${SRCS})
list(APPEND PUBLIC_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)

target_include_directories(${LIB_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS})
target_include_directories(${LIB_NAME} PRIVATE ${PRIVATE_INCLUDE_DIRS})

target_link_libraries(${LIB_NAME} PUBLIC ${PUBLIC_LIBRARIES})
target_link_libraries(${LIB_NAME} PRIVATE ${PRIVATE_LIBRARIES})
