#pragma once
#include "IDisplay.h"

namespace VORTEK_EDITOR
{
	class MenuDisplay : public IDisplay
	{
	private:
	public:
		MenuDisplay() = default;
		virtual ~MenuDisplay() = default;

		virtual void Draw() override;
	};
} // namespace VORTEK_EDITOR