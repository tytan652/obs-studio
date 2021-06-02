project(obs-cmocka)

find_package(CMocka CONFIG REQUIRED)

# Serializer test
add_executable(test_serializer test_serializer.c)
target_include_directories(test_serializer PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_serializer PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})

add_test(test_serializer ${CMAKE_CURRENT_BINARY_DIR}/test_serializer)
# fixLink(test_serializer)

# darray test
add_executable(test_darray test_darray.c)
target_include_directories(test_darray PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_darray PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})

add_test(test_darray ${CMAKE_CURRENT_BINARY_DIR}/test_darray)
# fixLink(test_darray)

# bitstream test
add_executable(test_bitstream test_bitstream.c)
target_include_directories(test_bitstream PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_bitstream PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})

add_test(test_bitstream ${CMAKE_CURRENT_BINARY_DIR}/test_bitstream)
# fixLink(test_bitstream)

# Fix libobs path macro(fixLink target_arg) if(APPLE AND UNIX)
# add_custom_command (TARGET ${target_arg} POST_BUILD COMMAND
# "${CMAKE_INSTALL_NAME_TOOL}" "-change" "@rpath/libobs.0.dylib"
# "@executable_path/../../libobs/libobs.0.dylib" "$<TARGET_FILE:${target_arg}>"
# VERBATIM) endif() endmacro()

# set(CMAKE_MACOSX_RPATH TRUE) set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# list(APPEND CMAKE_INSTALL_RPATH "@loader_path/" "@executable_path/")

# include_directories(${CMOCKA_INCLUDE_DIR}) include_directories(SYSTEM
# "${CMAKE_SOURCE_DIR}/libobs")

# # fix rpath on linux if (UNIX AND NOT APPLE) set(CMAKE_INSTALL_RPATH
# "$ORIGIN";../../libobs) endif()
