#pragma once
#include <string>

namespace VORTEK_SOUNDS
{
struct SoundParams
{
	std::string name{};
	std::string description{};
	std::string filename{};
	double duration{ 0.0 };
};
} // namespace VORTEK_SOUNDS
