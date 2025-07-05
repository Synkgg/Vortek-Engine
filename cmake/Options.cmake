#
#	Options
#

#--------------- Preprocessor definitions -------------------#
if(VORTEK_OPENGL_DEBUG_CALLBACK)
	add_compile_definitions(VORTEK_OPENGL_DEBUG_CALLBACK)
	message ( "VORTEK engine will be built with embedded realtime OpenGL debugger." )
endif()

if(VORTEK_OPENGL_DEBUG_FORWARD_COMPATIBILITY)
	add_compile_definitions(VORTEK_OPENGL_DEBUG_FORWARD_COMPATIBILITY)
	message ( "VORTEK engine will be build with a strict OpenGL context. This will trigger warnings on deprecated OpenGL dunctions." )
endif()

if(VORTEK_OPENGL_FORCE_DISCRETE_GPU)
	add_compile_definitions(VORTEK_OPENGL_FORCE_DISCRETE_GPU)
	message ( "VORTEK engine will attempt to force discrete GPU on optimus laptops." )
endif()
