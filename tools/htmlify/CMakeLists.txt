#
# This is a CMake makefile.  You can find the cmake utility and
# information about it at http://www.cmake.org
#

#set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
cmake_minimum_required(VERSION 2.6)
if(COMMAND cmake_policy) 
   cmake_policy(SET CMP0003 NEW) 
endif()

# create a variable called target_name and set it to the string "htmlify"
set (target_name htmlify)

project(${target_name})

# add all the cpp files we want to compile to this list.  This tells
# cmake that they are part of our target (which is the executable named htmlify)
add_executable(${target_name} 
htmlify.cpp
to_xml.cpp
)

# add the folder containing the dlib folder to the include path
include_directories(${DLIB_SOURCE_DIR})

# Tell cmake to link our target executable to the non-gui version of the dlib
# library.
target_link_libraries(${target_name} dlib )

install(TARGETS ${target_name}
	RUNTIME DESTINATION bin
	)
