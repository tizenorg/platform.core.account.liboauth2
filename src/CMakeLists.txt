
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
#SET(LIBDIR "\${prefix}/lib")
#SET(INCLUDEDIR "\${prefix}/include ")
SET(VERSION_MAJOR 0)
SET(VERSION "${VERSION_MAJOR}.0.1")

SET(SRCS
	oauth2_error.c
	oauth2_manager.c
	oauth2_request.c
	oauth2_response.c
	oauth2_util.c
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

INCLUDE(FindPkgConfig)
if(DEFINED EXCLUDE_EWK_ERR)
    add_definitions(-DWITHOUT_EWK_ERROR)
endif()

if(USE_CHROMIUM_EFL)
	add_definitions(-DWITH_CHROMIUM)
	pkg_check_modules(clientpkgs REQUIRED dlog glib-2.0 gio-2.0 gio-unix-2.0 capi-base-common libcurl efl-extension ecore evas chromium-efl json-glib-1.0 elementary  tpkp-curl)
else()
	pkg_check_modules(clientpkgs REQUIRED dlog glib-2.0 gio-2.0 gio-unix-2.0 capi-base-common libcurl efl-extension ecore evas ewebkit2 json-glib-1.0 elementary  tpkp-curl)
endif()

FOREACH(flag ${clientpkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Wall -Werror")
SET(CMAKE_LDFLAGS "-Wl,-zdefs")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DFACTORYFS=\"$ENV{FACTORYFS}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION ${VERSION_MAJOR})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION ${VERSION})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${clientpkgs_LDFLAGS})

CONFIGURE_FILE(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)

#INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib)
#CONFIGURE_FILE(oauth2.pc.in ../oauth2.pc @ONLY)
#INSTALL(FILES ${CMAKE_BINARY_DIR}/oauth2.pc DESTINATION lib/pkgconfig)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIBDIR}/oauth2)
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIBDIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${LIBDIR}/pkgconfig)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/oauth2.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/oauth2_manager.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/oauth2_request.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/oauth2_response.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/oauth2_error.h DESTINATION include)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/oauth2_types.h DESTINATION include)
