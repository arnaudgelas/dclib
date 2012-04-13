project( DLIB )

cmake_minimum_required( VERSION 2.4 )

#set default build type to Release
include( ${DLIB_SOURCE_DIR}/cmake/release_build_by_default.cmake )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${arnaud_BINARY_DIR}/bin
)

add_subdirectory( dlib )
add_subdirectory( tools )
add_subdirectory( examples )
