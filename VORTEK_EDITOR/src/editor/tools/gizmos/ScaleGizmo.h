#pragma once
#include "Gizmo.h"

namespace VORTEK_EDITOR
{
	class ScaleGizmo : public Gizmo
	{
	public:
		ScaleGizmo();
		virtual void Update(struct Canvas& canvas) override;
		virtual void Draw() override;
	};
} // namespace VORTEK_EDITOR