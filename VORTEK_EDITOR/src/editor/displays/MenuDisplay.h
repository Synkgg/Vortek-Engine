#pragma once
#include "IDisplay.h"

namespace VORTEK_EDITOR
{
enum class EDisplay : uint64_t;
class EditorState;

class MenuDisplay : public IDisplay
{
  public:
	MenuDisplay() = default;
	virtual ~MenuDisplay() = default;

	virtual void Draw() override;

  private:
	void DrawDisplayItem( EditorState& editorState, const std::string& sDisplayName, const EDisplay eDisplay );
};
} // namespace VORTEK_EDITOR
