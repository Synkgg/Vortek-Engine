#pragma once
#include "Gizmo.h"

namespace Vortek::Editor
{
class ScaleGizmo : public Gizmo
{
  public:
	ScaleGizmo();
	virtual void Update( Vortek::Core::Canvas& canvas ) override;
	virtual void Draw( Vortek::Rendering::Camera2D* pCamera ) override;
};
} // namespace Vortek::Editor
