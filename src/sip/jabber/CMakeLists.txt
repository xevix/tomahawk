project( tomahawk )

include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_SHARED )
add_definitions( -DDLLEXPORT_PRO )

set( jabberSources
        jabber.cpp
	jabber_p.cpp
)

set( jabberHeaders
	jabber.h
        jabber_p.h
)

include_directories( . ${CMAKE_CURRENT_BINARY_DIR} .. 
   ${QT_INCLUDE_DIR}
)

qt4_wrap_cpp( jabberMoc ${jabberHeaders} )
add_library( sip_jabber SHARED ${jabberSources} ${jabberMoc} )

IF( WIN32 )
SET( OS_SPECIFIC_LINK_LIBRARIES
	${OS_SPECIFIC_LINK_LIBRARIES}
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/src/libtomahawk/libtomahawklib.dll"
)
ENDIF( WIN32 )

target_link_libraries( sip_jabber
	${QT_LIBRARIES}
        ${GLOOX_LIBS}
	${OS_SPECIFIC_LINK_LIBRARIES}
)

IF( APPLE )
  SET( CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-undefined dynamic_lookup" )
ENDIF( APPLE )

install( TARGETS sip_jabber DESTINATION lib )