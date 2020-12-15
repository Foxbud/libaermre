# Copyright 2020 the libaermre authors
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
cmake_minimum_required(VERSION 3.18)

project(AERMRE
	VERSION 1.0.0
	DESCRIPTION "AER modding framework for Hyper Light Drifter"
	LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} cmake)

find_package(Foxutils REQUIRED)
find_package(Doxygen)

add_library(aermre SHARED
	src/confvars.c
	src/envconf.c
	src/err.c
	src/eventkey.c
	src/eventtrap.c
	src/hld.c
	src/instance.c
	src/log.c
	src/modman.c
	src/mre.c
	src/object.c
	src/objtree.c
	src/rand.c
	src/room.c
	src/sprite.c)
target_include_directories(aermre PRIVATE include/private)
target_include_directories(aermre PUBLIC include/public)
target_compile_options(aermre 
	PRIVATE -m32
	PRIVATE -Wall
	PRIVATE -Wextra
	PRIVATE -Werror
	PRIVATE -O3)
target_link_options(aermre
	PRIVATE -m32
	PRIVATE -rdynamic)
target_link_libraries(aermre Foxutils::foxutils)

if(DOXYGEN_FOUND)
	set(DOXYGEN_HAVE_DOT FALSE)
	set(DOXYGEN_SHOW_INCLUDE_FILES FALSE)
	set(DOXYGEN_TYPEDEF_HIDES_STRUCT TRUE)
	set(DOXYGEN_STRIP_FROM_PATH "include/public")
	get_target_property(AERMRE_INTERFACE_INCLUDE_DIRECTORIES
		aermre
		INTERFACE_INCLUDE_DIRECTORIES)
	doxygen_add_docs(aermre-docs ${AERMRE_INTERFACE_INCLUDE_DIRECTORIES})
endif()
