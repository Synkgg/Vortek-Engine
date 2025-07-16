#pragma once
#include <string>

#define BASE_PATH                                                                                                      \
	std::string                                                                                                        \
	{                                                                                                                  \
		SDL_GetBasePath()                                                                                              \
	}

#ifdef _WIN32
constexpr char PATH_SEPARATOR = '\\';
#define DEFAULT_PROJECT_PATH BASE_PATH + "Vortek Projects"
#else
constexpr char PATH_SEPARATOR = '/';
#define DEFAULT_PROJECT_PATH BASE_PATH + PATH_SEPARATOR + "Vortek Projects"
#endif

#define SCRIPTS "scripts"
#define ASSETS "assets"
#define CONTENT "content"

constexpr const std::string_view CONTENT_FOLDER = "content";
constexpr const std::string_view PRJ_FILE_EXT = ".veprj";

namespace VORTEK_UTIL
{

struct SpriteLayerParams
{
	std::string sLayerName{ "NewLayer" };
	bool bVisible{ true };
};

} // namespace VORTEK_UTIL
