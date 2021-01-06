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
find_path(TOMLC99_INCLUDE_DIR
	NAMES "toml.h"
	PATHS
		"$ENV{HOME}/.local/include"
		"/usr/local/include"
		"/usr/include"
	REQUIRED
	NO_DEFAULT_PATH)
find_library(TOMLC99_LIBRARY
	NAMES "libtoml.a"
	PATHS
		"$ENV{HOME}/.local/lib32"
		"/usr/local/lib32"
		"/usr/lib32"
		"$ENV{HOME}/.local/lib"
		"/usr/local/lib"
		"/usr/lib"
	REQUIRED
	NO_DEFAULT_PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Tomlc99
	FOUND_VAR TOMLC99_FOUND
	REQUIRED_VARS
		TOMLC99_INCLUDE_DIR
		TOMLC99_LIBRARY)

if(TOMLC99_FOUND AND NOT TARGET Tomlc99::tomlc99)
	add_library(Tomlc99::tomlc99 STATIC IMPORTED)
	set_target_properties(Tomlc99::tomlc99 PROPERTIES
		IMPORTED_LOCATION "${TOMLC99_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${TOMLC99_INCLUDE_DIR}")
endif()
