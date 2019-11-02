PROJECT( XPlanePositionProviderPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

SET( xplane_SRCS
 XPlanePositionProviderPlugin.cpp
)

SET( XPlanePositionProviderPlugin_LIBS
)

MARBLE_ADD_PLUGIN( XPlanePositionProviderPlugin ${xplane_SRCS} )
