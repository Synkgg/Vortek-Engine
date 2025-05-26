add_library(VORTEK_SOUNDS
    "src/Sounds/Essentials/Music.h"
    "src/Sounds/Essentials/Music.cpp"
    "src/Sounds/Essentials/SoundFX.h"
    "src/Sounds/Essentials/SoundFX.cpp"
    "src/Sounds/Essentials/SoundParams.h"

    "src/Sounds/MusicPlayer/MusicPlayer.h"
    "src/Sounds/MusicPlayer/MusicPlayer.cpp"

    "src/Sounds/SoundPlayer/SoundFxPlayer.h"
    "src/Sounds/SoundPlayer/SoundFxPlayer.cpp"
)

target_include_directories(
    VORTEK_SOUNDS
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
	VORTEK_SOUNDS
    PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    PUBLIC VORTEK_UTILITIES
)

target_compile_options(
    VORTEK_SOUNDS PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
