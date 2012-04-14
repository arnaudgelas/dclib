project( DLIB )

cmake_minimum_required( VERSION 2.6 )

#set default build type to Release
include( ${DLIB_SOURCE_DIR}/cmake/release_build_by_default.cmake )

include( CTest )

# -------------------------------------------------------------------------
# Define CMAKE_*_OUTPUT_DIRECTORY

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${DLIB_BINARY_DIR}/bin
)

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${DLIB_BINARY_DIR}/lib
)

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${DLIB_BINARY_DIR}/lib
)

add_subdirectory( dlib )
add_subdirectory( tools )
add_subdirectory( examples )
