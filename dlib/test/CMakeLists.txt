#
# This is a CMake makefile.  You can find the cmake utility and
# information about it at http://www.cmake.org
#

cmake_minimum_required(VERSION 2.6)

include_directories( ${DLIB_SOURCE_DIR} )

# This variable contains a list of all the tests we are building
# into the regression test suite.
set (test_names
   example
   example_args
   any
   any_function
   array2d
   array
   assignment_learning
   base64
   bayes_nets
   bigint
   binary_search_tree_kernel_1a
   binary_search_tree_kernel_2a
   binary_search_tree_mm1
   binary_search_tree_mm2
   bridge
   byte_orderer
   cmd_line_parser
   cmd_line_parser_wchar_t
   compress_stream
   conditioning_class_c
   conditioning_class
   config_reader
   crc32
   create_iris_datafile
   data_io
   directed_graph
   discriminant_pca
   disjoint_subsets
   ekm_and_lisf
   empirical_kernel_map
   entropy_coder
   entropy_encoder_model
   filtering
   find_max_factor_graph_nmplp
   find_max_factor_graph_viterbi
   geometry
   graph
   hash
   hash_map
   hash_set
   hash_table
   hog_image
   image
   is_same_object
   kcentroid
   kernel_matrix
   kmeans
   least_squares
   linear_manifold_regularizer
   lz77_buffer
   map
   matrix2
   matrix3
   matrix4
   matrix_chol
   matrix
   matrix_eig
   matrix_lu
   matrix_qr
   max_cost_assignment
   max_sum_submatrix
   md5
   member_function_pointer
   metaprogramming
   multithreaded_object
   object_detector
   one_vs_all_trainer
   one_vs_one_trainer
   optimization
   optimization_test_functions
   opt_qp_solver
   pipe
   pixel
   probabilistic
   pyramid_down
   queue
   rand
   read_write_mutex
   reference_counter
   rls
   scan_image
   sequence
   sequence_labeler
   serialize
   set
   sldf
   sliding_buffer
   smart_pointers
   sockets2
   sockets
   sockstreambuf
   stack
   static_map
   static_set
   statistics
   std_vector_c
   string
   svm_c_linear
   svm
   svm_multiclass_linear
   svm_struct
   symmetric_matrix_cache
   thread_pool
   threads
   timer
   tokenizer
   trust_region
   tuple
   type_safe_union
   )

set( tests_CXX )
foreach( _test ${test_names} )
  set( tests_CXX ${tests_CXX} ${_test}.cpp )
endforeach()

# create a variable called target_name and set it to the string "test"
set (target_name test)

project(${target_name})

# add all the cpp files we want to compile to this list.  This tells
# cmake that they are part of our target (which is the executable named test)
add_executable(${target_name} main.cpp tester.cpp ${tests_CXX})

if (NOT ${DLIB_NO_GUI_SUPPORT})
   add_subdirectory(gui)
endif()

# Tell cmake to link our target executable to the non-gui version of the dlib
# library.
target_link_libraries(${target_name} dlib )

foreach( _test ${test_names} )
  add_test( NAME test_${_test} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name} --test_${_test} )
endforeach()
