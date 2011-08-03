#SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tomahawk.app/Contents/Plugins/" )

MACRO(add_plugin pluginName )
    MESSAGE(STATUS "Fixup ${pluginName}")
    LIST(APPEND PLUGIN_LIST "${CMAKE_BINARY_DIR}/_CPack_Packages/OSX/DragNDrop/Tomahawk-${TOMAHAWK_VERSION}/Tomahawk.app/Contents/Plugins/lib${pluginName}.dylib")
ENDMACRO()


IF( LIBJREEN_FOUND )
    ADD_SUBDIRECTORY( jabber )
ENDIF( LIBJREEN_FOUND )

IF( QTWEETLIB_FOUND )
    #ADD_SUBDIRECTORY( twitter )
ENDIF( QTWEETLIB_FOUND )

ADD_SUBDIRECTORY( zeroconf )
