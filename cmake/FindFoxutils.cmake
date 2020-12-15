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
find_path(FOXUTILS_INCLUDE_DIR
	NAMES "foxutils/array.h"
	PATHS "$ENV{HOME}/.local/include"
	REQUIRED)
find_library(FOXUTILS_LIBRARY
	NAMES "libfoxutils.a"
	PATHS "$ENV{HOME}/.local/lib32"
	REQUIRED)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Foxutils
	FOUND_VAR FOXUTILS_FOUND
	REQUIRED_VARS
		FOXUTILS_INCLUDE_DIR
		FOXUTILS_LIBRARY)

if(FOXUTILS_FOUND AND NOT TARGET Foxutils::foxutils)
	add_library(Foxutils::foxutils STATIC IMPORTED)
	set_target_properties(Foxutils::foxutils PROPERTIES
		IMPORTED_LOCATION "${FOXUTILS_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${FOXUTILS_INCLUDE_DIR}")
endif()
