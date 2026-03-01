#pragma once
#include <string>

namespace Vortek::Sounds
{
struct SoundParams
{
	std::string name{};
	std::string description{};
	std::string filename{};
	double duration{ 0.0 };
};
} // namespace Vortek::Sounds
