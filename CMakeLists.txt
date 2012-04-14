project( DLIB )

cmake_minimum_required( VERSION 2.6 )

#set default build type to Release
include( ${DLIB_SOURCE_DIR}/cmake/release_build_by_default.cmake )

include( CTest )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${DLIB_BINARY_DIR}/bin
)

add_subdirectory( dlib )
add_subdirectory( tools )
add_subdirectory( examples )
