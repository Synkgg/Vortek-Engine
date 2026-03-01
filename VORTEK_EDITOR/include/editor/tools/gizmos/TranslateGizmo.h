#pragma once
#include "Gizmo.h"

namespace Vortek::Editor
{
class TranslateGizmo : public Gizmo
{
  public:
	TranslateGizmo();
	virtual void Update( Vortek::Core::Canvas& canvas ) override;
	virtual void Draw( Vortek::Rendering::Camera2D* pCamera ) override;
};
} // namespace Vortek::Editor
