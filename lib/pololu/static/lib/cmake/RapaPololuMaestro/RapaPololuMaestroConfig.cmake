SET( TARGET_NAME "RapaPololuMaestro" )
SET( EXTRA_SYSTEM_INCLUDE_DIRS "" )
INCLUDE( "${CMAKE_CURRENT_LIST_DIR}/${TARGET_NAME}Targets.cmake" )
SET( ${TARGET_NAME}_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../include" ${EXTRA_SYSTEM_INCLUDE_DIRS} )
#SET( ${TARGET_NAME}_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/../../${TARGET_NAME}" )	 # Not needed. Importing the target will automatically TARGET_LINK_LIBRARIES it and its dependencies
