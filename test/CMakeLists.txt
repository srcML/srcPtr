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

#enable_testing()
#add_test( Testtest.cpp
file( GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Test* )
foreach( testsourcefile ${SOURCES} )
    string( REPLACE ".cpp" "" testname ${testsourcefile} )
    get_filename_component(file ${testsourcefile} NAME_WE)
    add_executable( ${file} ${testsourcefile} )
    target_link_libraries( ${file} srcsaxeventdispatch srcsax_static srcml ${LIBXML2_LIBRARIES} )
endforeach( testsourcefile ${SOURCES} )

