#pragma once
#include "Gizmo.h"

namespace VORTEK_EDITOR
{
	class RotateGizmo : public Gizmo
	{
	public:
		RotateGizmo();
		virtual void Update(struct Canvas& canvas) override;
		virtual void Draw() override;
	};
} // namespace VORTEK_EDITOR