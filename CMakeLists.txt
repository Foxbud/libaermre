cmake_minimum_required(VERSION 3.12)
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

project(AERMRE
    VERSION 1.5.1
    DESCRIPTION "AER modding framework for Hyper Light Drifter."
    LANGUAGES C
)

# Handle pre-release version.
set(PRE_RELEASE FALSE)
if(PRE_RELEASE)
    set(EXTENDED_PROJECT_VERSION "${CMAKE_PROJECT_VERSION}-pre")
else()
    set(EXTENDED_PROJECT_VERSION "${CMAKE_PROJECT_VERSION}")
endif()

# Set cmake defaults.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "libaermre-${EXTENDED_PROJECT_VERSION}"
        CACHE PATH "..." FORCE
    )
endif()

# Include CPack.
set(CPACK_GENERATOR "TXZ")
set(CPACK_PACKAGE_FILE_NAME "libaermre-${EXTENDED_PROJECT_VERSION}")
include(CPack)

# Include GenerateExportHeader.
include(GenerateExportHeader)

find_package(Foxutils REQUIRED)
find_package(Tomlc99 REQUIRED)
find_package(Doxygen)

# Configure files.
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/VERSION.txt.in"
    "${CMAKE_CURRENT_BINARY_DIR}/VERSION.txt"
    @ONLY
)

# Add MRE library target.
add_library(aermre SHARED
   src/conf.c
   src/core.c
   src/draw.c
   src/err.c
   src/event.c
   src/font.c
   src/hld.c
   src/input.c
   src/instance.c
   src/log.c
   src/mod.c
   src/object.c
   src/option.c
   src/rand.c
   src/room.c
   src/save.c
   src/sprite.c
)
generate_export_header(aermre
    BASE_NAME AER
    EXPORT_FILE_NAME include/private/internal/export.h
)
set_target_properties(aermre PROPERTIES
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
)
target_include_directories(aermre
    PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/private"
    "${CMAKE_CURRENT_BINARY_DIR}/include/private"
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public>"
    "$<INSTALL_INTERFACE:include>"
)
target_compile_options(aermre 
    PRIVATE -m32 -Wall -Wextra -Werror -Wfatal-errors
)
target_link_options(aermre
    PRIVATE -m32
    PRIVATE -rdynamic
)
target_link_libraries(aermre
    PRIVATE Foxutils::foxutils
    PRIVATE Tomlc99::tomlc99
)

# Add installation target.
install(TARGETS aermre
    EXPORT AERMRETargets
    LIBRARY DESTINATION lib
)
install(EXPORT AERMRETargets
    FILE AERMRETargets.cmake
    NAMESPACE AERMRE::
    DESTINATION lib/cmake
)
install(FILES
    AUTHORS.txt
    LICENSE.txt
    NOTICE.txt
    "${CMAKE_CURRENT_BINARY_DIR}/VERSION.txt"
    DESTINATION "."
)
install(DIRECTORY ACKNOWLEDGMENTS
    DESTINATION "."
)
install(DIRECTORY include/public/aer
    DESTINATION include
)

if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NUMBER "${EXTENDED_PROJECT_VERSION}")
    set(DOXYGEN_HAVE_DOT FALSE)
    set(DOXYGEN_SHOW_INCLUDE_FILES FALSE)
    set(DOXYGEN_TYPEDEF_HIDES_STRUCT TRUE)
    set(DOXYGEN_ENABLE_PREPROCESSING FALSE)
    set(DOXYGEN_STRIP_FROM_PATH "include/public")
    set(DOXYGEN_SEARCHENGINE FALSE)
    set(DOXYGEN_DISABLE_INDEX TRUE)
    set(DOXYGEN_GENERATE_TREEVIEW TRUE)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET
        "doxygen-awesome/doxygen-awesome.css"
        "doxygen-awesome/doxygen-awesome-sidebar-only.css"
    )
    doxygen_add_docs(docs
        "${CMAKE_CURRENT_SOURCE_DIR}/include/public"
        "${CMAKE_CURRENT_SOURCE_DIR}/docs"
    )
endif()
