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
