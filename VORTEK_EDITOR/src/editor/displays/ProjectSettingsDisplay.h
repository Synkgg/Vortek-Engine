#pragma once
#include "IDisplay.h"
#include <string>
#include <functional>
#include <vector>

namespace VORTEK_CORE
{
class ProjectInfo;
class CoreEngineData;
namespace ECS
{
class MainRegistry;
}
} // namespace VORTEK_CORE

namespace VORTEK_EDITOR
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

	SettingCategory CreateGeneralSettings( VORTEK_CORE::CoreEngineData& coreGlobals,
										   VORTEK_CORE::ProjectInfo& projectInfo,
										   VORTEK_CORE::ECS::MainRegistry& mainRegistry );

	SettingCategory CreatePhysicsSettings( VORTEK_CORE::CoreEngineData& coreGlobals,
										   VORTEK_CORE::ProjectInfo& projectInfo,
										   VORTEK_CORE::ECS::MainRegistry& mainRegistry );

	SettingCategory CreateGraphicsSettings( VORTEK_CORE::CoreEngineData& coreGlobals,
											VORTEK_CORE::ProjectInfo& projectInfo,
											VORTEK_CORE::ECS::MainRegistry& mainRegistry );

	SettingCategory CreateAudioSettings( VORTEK_CORE::CoreEngineData& coreGlobals, VORTEK_CORE::ProjectInfo& projectInfo,
										 VORTEK_CORE::ECS::MainRegistry& mainRegistry );

  private:
	SettingCategory m_SettingsCategory;
	std::string m_sSelectedCategory;
};
} // namespace VORTEK_EDITOR
