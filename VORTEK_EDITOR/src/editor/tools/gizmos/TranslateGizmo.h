#pragma once
#include "Gizmo.h"

namespace VORTEK_EDITOR
{
	class TranslateGizmo : public Gizmo
	{
	public:
		TranslateGizmo();
		virtual void Update(struct Canvas& canvas) override;
		virtual void Draw() override;
	};
} // namespace VORTEK_EDITOR