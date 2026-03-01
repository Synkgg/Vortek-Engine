#pragma once
#include <string>

#define BASE_PATH                                                                                                      \
	std::string                                                                                                        \
	{                                                                                                                  \
		SDL_GetBasePath()                                                                                              \
	}

#ifdef _WIN32
constexpr char PATH_SEPARATOR = '\\';
#define DEFAULT_PROJECT_PATH BASE_PATH + "VortekProjects"
#else
constexpr char PATH_SEPARATOR = '/';
#define DEFAULT_PROJECT_PATH BASE_PATH + PATH_SEPARATOR + "VortekProjects"
#endif

#define SCRIPTS "scripts"
#define ASSETS "assets"
#define CONTENT "content"

constexpr const std::string_view CONTENT_FOLDER = "content";
constexpr const std::string_view PRJ_FILE_EXT = ".veprj";

namespace Vortek::Utilities
{

struct SpriteLayerParams
{
	std::string sLayerName{ "NewLayer" };
	bool bVisible{ true };
	int layer{ -1 };

	friend bool operator==(const SpriteLayerParams& a, const SpriteLayerParams& b)
	{
		return a.sLayerName == b.sLayerName && a.bVisible == b.bVisible && a.layer == b.layer;
	}
};

} // namespace Vortek::Utilities
