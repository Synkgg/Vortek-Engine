#pragma once
#include "Gizmo.h"

namespace VORTEK_EDITOR
{
class RotateGizmo : public Gizmo
{
  public:
	RotateGizmo();
	virtual void Update( VORTEK_CORE::Canvas& canvas ) override;
	virtual void Draw( VORTEK_RENDERING::Camera2D* pCamera ) override;
};
} // namespace VORTEK_EDITOR
