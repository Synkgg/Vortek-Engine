#pragma once
#include "IDisplay.h"
#include <memory>
#include <string>

namespace VORTEK_CORE
{
struct GameConfig;
}

namespace VORTEK_EDITOR
{
class PackageGameDisplay : public IDisplay
{
  public:
	PackageGameDisplay();
	~PackageGameDisplay();

	virtual void Update() override;
	virtual void Draw() override;

  private:
	std::unique_ptr<VORTEK_CORE::GameConfig> m_pGameConfig;
	std::string m_sDestinationPath;
	std::string m_sScriptListPath;
	std::string m_sFileIconPath;

	bool m_bResizable;
	bool m_bBorderless;
	bool m_bFullscreen;
	bool m_bTitlebar;

	bool m_bScriptListExist;
};
} // namespace VORTEK_EDITOR
