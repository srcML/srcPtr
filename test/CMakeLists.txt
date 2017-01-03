##
# CMakeLists.txt
#
# Copyright (C) 2016 srcML, LLC. (www.srcML.org)
#
# This file is part of srcPtr.
#
# srcPtr is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# srcPtr is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with srcPtr.  If not, see <http://www.gnu.org/licenses/>.

macro(add_srcptr_test TEST_FILE)

    get_filename_component(TEST_NAME_WITH_EXTENSION ${TEST_FILE} NAME)
    string(FIND ${TEST_NAME_WITH_EXTENSION} "." EXTENSION_BEGIN)
    string(SUBSTRING ${TEST_NAME_WITH_EXTENSION} 0 ${EXTENSION_BEGIN} TEST_NAME)

    add_definitions("-std=c++1y -g")
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} srcsaxeventdispatch srcsax_static srcml ${LIBXML2_LIBRARIES} ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

  endmacro()

add_srcptr_test(TestsrcPtrUtilities.cpp)
add_srcptr_test(TestsrcPtrPolicyData.cpp)
add_srcptr_test(TestsrcPtrDeclPolicy.cpp)
add_srcptr_test(TestsrcPtrPolicy.cpp)
