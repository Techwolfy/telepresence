#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Rapa::RapaPololuMaestro" for configuration ""
set_property(TARGET Rapa::RapaPololuMaestro APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(Rapa::RapaPololuMaestro PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libRapaPololuMaestro.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Rapa::RapaPololuMaestro )
list(APPEND _IMPORT_CHECK_FILES_FOR_Rapa::RapaPololuMaestro "${_IMPORT_PREFIX}/lib/libRapaPololuMaestro.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
