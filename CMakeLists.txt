##
# CMakeLists.txt
#
# Copyright (C) 2015-2016 srcML, LLC. (www.srcML.org)
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

cmake_minimum_required(VERSION 2.8)
project(srcPtr)

enable_testing()

# find needed libraries
find_package(LibXml2 REQUIRED)

# include needed includes
include_directories(${LIBXML2_INCLUDE_DIR})
add_definitions("-std=c++1y")

# Download the CLI11 parser into directory named "external".
set(CMAKE_EXTERNAL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external) # Creates folder if doesn't exists.
# Name of file is needed since content of file itself are being downloaded.
file(DOWNLOAD https://github.com/CLIUtils/CLI11/releases/download/v2.3.2/CLI11.hpp ${CMAKE_EXTERNAL_SOURCE_DIR}/CLI11.hpp)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src
                    external
                    ${CMAKE_SOURCE_DIR}/srcSAX/src/srcsax
                    ${CMAKE_SOURCE_DIR}/srcSAX/src/cpp
                    ${CMAKE_SOURCE_DIR}/srcSAX/src/windows
                    ${CMAKE_SOURCE_DIR}/srcSAXEventDispatch/src/dispatcher
                    ${CMAKE_SOURCE_DIR}/srcSAXEventDispatch/src/policy_classes
                    )

add_subdirectory(srcSAX/src)
add_subdirectory(srcSAXEventDispatch/src)
add_subdirectory(src)

add_subdirectory(test)
