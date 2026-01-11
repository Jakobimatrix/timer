
# change the version if there are API changes!
set(LIB_VERSION "1.0.0")
# set the name of your library here!
set(LIB_NAME "timer_lib")

set(LIBRARY_LIB_VERSION ${LIB_VERSION})

add_library(${LIB_NAME}_${LIBRARY_LIB_VERSION} INTERFACE)

target_link_libraries(${LIB_NAME}_${LIBRARY_LIB_VERSION}
  INTERFACE
  BuildSettings_LIB
)

target_include_directories(${LIB_NAME}_${LIBRARY_LIB_VERSION} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS ${LIB_NAME}_${LIBRARY_LIB_VERSION}
  EXPORT ${LIB_NAME}Targets
)