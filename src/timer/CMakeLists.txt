
# change the version if there are API changes!
set(LIB_VERSION "0.0.1")
# set the name of your library here!
set(LIB_NAME "timer_lib")

if(NOT DEFINED LIBRARY_LIB_VERSION)
    set(LIBRARY_LIB_VERSION ${LIB_VERSION})

    add_library(${LIB_NAME}_${LIBRARY_LIB_VERSION} INTERFACE)

    target_include_directories(${LIB_NAME}_${LIBRARY_LIB_VERSION} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

elseif(LIBRARY_LIB_VERSION STREQUAL LIB_VERSION)
    # LIBRARY_LIB_VERSION is defined and matches LIB_VERSION
    # Do nothing

else()
    # LIBRARY_LIB_VERSION does not equal LIB_VERSION
    message(FATAL_ERROR "${LIB_NAME} already included with a different version. This Version: ${LIB_VERSION}, Other Version: ${LIBRARY_LIB_VERSION}")
endif()

