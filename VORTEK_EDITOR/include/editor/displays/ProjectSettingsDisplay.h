#pragma once
#include "IDisplay.h"

namespace Vortek::Core
{
class ProjectInfo;
class CoreEngineData;
namespace ECS
{
class MainRegistry;
}
} // namespace Vortek::Core

namespace Vortek::Editor
{
class ProjectSettingsDisplay : public IDisplay
{
  public:
	ProjectSettingsDisplay();
	~ProjectSettingsDisplay();

	virtual void Update() override;
	virtual void Draw() override;

  private:
	struct SettingItem
	{
		std::string sName{};
		std::function<void()> drawFunc;
	};

	struct SettingCategory
	{
		std::string sName;
		std::vector<SettingItem> items;
		std::vector<SettingCategory> subCategories;
	};

	void DrawCategoryTree( const SettingCategory& category, std::string& selected );
	void DrawSettingsPanel( const SettingCategory& category, std::string& selected );
	void CreateProjectSettings();

	SettingCategory CreateGeneralSettings( Vortek::Core::CoreEngineData& coreGlobals,
										   Vortek::Core::ProjectInfo& projectInfo,
										   Vortek::Core::ECS::MainRegistry& mainRegistry );

	SettingCategory CreatePhysicsSettings( Vortek::Core::CoreEngineData& coreGlobals,
										   Vortek::Core::ProjectInfo& projectInfo,
										   Vortek::Core::ECS::MainRegistry& mainRegistry );

	SettingCategory CreateGraphicsSettings( Vortek::Core::CoreEngineData& coreGlobals,
											Vortek::Core::ProjectInfo& projectInfo,
											Vortek::Core::ECS::MainRegistry& mainRegistry );

	SettingCategory CreateAudioSettings( Vortek::Core::CoreEngineData& coreGlobals, Vortek::Core::ProjectInfo& projectInfo,
										 Vortek::Core::ECS::MainRegistry& mainRegistry );

  private:
	SettingCategory m_SettingsCategory;
	std::string m_sSelectedCategory;
};
} // namespace Vortek::Editor
