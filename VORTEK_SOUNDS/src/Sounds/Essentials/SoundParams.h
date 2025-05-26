#pragma once
#include <string>
namespace VORTEK_SOUNDS {
	struct SoundParams
	{
		std::string name{ "" }, description{ "" }, filename{ "" };
		double duration{ 0.0 };
	};
}