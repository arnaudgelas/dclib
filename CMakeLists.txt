project( arnaud )

cmake_minimum_required( VERSION 2.4 )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${arnaud_BINARY_DIR}/bin
)

add_subdirectory( tools )
add_subdirectory( examples )
