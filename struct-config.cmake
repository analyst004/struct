# Library configuration file used by dependent projects
# via find_package() built-in directive in "config" mode.

if(NOT DEFINED STRUCT_FOUND)

  # Locate library headers.
  FIND_PATH(STRUCT_INCLUDE_DIRS 
    NAMES slist.h
    PATHS ${STRUCT_DIR} 
    NO_DEFAULT_PATH
  )

  # Common name for exported library targets.
  SET(STRUCT_LIBRARIES
    struct
    CACHE INTERNAL "struct library" FORCE
  )

  # Usual "required" et. al. directive logic.
  INCLUDE(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(
    struct DEFAULT_MSG
    STRUCT_INCLUDE_DIRS
    STRUCT_LIBRARIES
  )

  # Add targets to dependent project.
  add_subdirectory(
    ${STRUCT_DIR}
    ${CMAKE_BINARY_DIR}/struct
  )
endif()
