FIND_PACKAGE(TULIP REQUIRED)
FIND_PACKAGE(QtX REQUIRED)

SET(PLUGIN_NAME Infiniband)

if(CMAKE_VERSION VERSION_LESS 2.8.13)
  get_property(LIBIBAUTILS_LOCATION TARGET libibautils PROPERTY LOCATION)
else(CMAKE_VERSION VERSION_LESS 2.8.13)
  set(LIBIBAUTILS_LOCATION $<TARGET_FILE:libibautils>)
endif(CMAKE_VERSION VERSION_LESS 2.8.13)

if(LIBIBAUTILS_LOCATION)                                                                                                                                                                                                                                         
    #MESSAGE(STATUS "Adding Infiniband Import Plugin. Linking against ${LIBIBAUTILS_LOCATION}")                                                                                                                                                                  
    MESSAGE(STATUS "Adding Infiniband Plugins.")
    INCLUDE_DIRECTORIES(${RE2_INCLUDE_DIR} ${libibautilInclude} ${TULIP_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR})

    ADD_LIBRARY(${PLUGIN_NAME}-${TULIP_VERSION} SHARED routes.cpp topology.cpp fabric.cpp csv.cpp )
    TARGET_LINK_LIBRARIES(${PLUGIN_NAME}-${TULIP_VERSION} ${TULIP_CORE_LIBRARY} ${TULIP_GUI_LIBRARY} ${LIBIBAUTILS})
    INSTALL(TARGETS ${PLUGIN_NAME}-${TULIP_VERSION} DESTINATION ${TULIP_PLUGINS_DIR})

else(LIBIBAUTILS_LOCATION)                                                                                                                                                                                                                                       
    message(STATUS "Skipping Infiniband Plugins")                                                                                                                                                                                                          
endif(LIBIBAUTILS_LOCATION)
