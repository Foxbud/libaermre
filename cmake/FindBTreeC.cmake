# Copyright 2021 the libaermre authors
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
find_path(BTREEC_INCLUDE_DIR
    NAMES "btree.h"
    PATHS
        "$ENV{HOME}/.local/include"
        "/usr/local/include"
        "/usr/include"
    REQUIRED
    NO_DEFAULT_PATH)
find_library(BTREEC_LIBRARY
    NAMES "libbtree.a"
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
find_package_handle_standard_args(BTreeC
    FOUND_VAR BTREEC_FOUND
    REQUIRED_VARS
        BTREEC_INCLUDE_DIR
        BTREEC_LIBRARY)

if(BTREEC_FOUND AND NOT TARGET BTreeC::btree)
    add_library(BTreeC::btree STATIC IMPORTED)
    set_target_properties(BTreeC::btree PROPERTIES
        IMPORTED_LOCATION "${BTREEC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${BTREEC_INCLUDE_DIR}")
endif()
