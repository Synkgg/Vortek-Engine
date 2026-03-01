#
#	Options
#

#--------------- Preprocessor definitions -------------------#
if(VORTEK_OPENGL_DEBUG_CALLBACK)
	add_compile_definitions(VORTEK_OPENGL_DEBUG_CALLBACK)
	message ( "Vortek engine will be built with embedded realtime OpenGL debugger." )
endif()

if(VORTEK_OPENGL_DEBUG_FORWARD_COMPATIBILITY)
	add_compile_definitions(VORTEK_OPENGL_DEBUG_FORWARD_COMPATIBILITY)
	message ( "Vortek engine will be build with a strict OpenGL context. This will trigger warnings on deprecated OpenGL dunctions." )
endif()

if(VORTEK_OPENGL_FORCE_DISCRETE_GPU)
	add_compile_definitions(VORTEK_OPENGL_FORCE_DISCRETE_GPU)
	message ( "Vortek engine will attempt to force discrete GPU on optimus laptops." )
endif()
