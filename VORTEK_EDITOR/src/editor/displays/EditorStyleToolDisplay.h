#pragma once
#include "IDisplay.h"

namespace VORTEK_EDITOR
{

class EditorStyleToolDisplay : public IDisplay
{

  public:
	EditorStyleToolDisplay();
	virtual ~EditorStyleToolDisplay() = default;

	virtual void Draw() override;

  private:
	void DrawSizePicker();
	void DrawColorPicker();
};
} // namespace VORTEK_EDITOR
