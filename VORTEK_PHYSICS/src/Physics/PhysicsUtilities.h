#pragma once
#include <cstdint>
#include <string>
#include <map>

namespace VORTEK_PHYSICS
{

	enum class RigidBodyType
	{
		STATIC = 0, // zero mass, zero velocity, may be manually moved
		KINEMATIC,	// zero mass, velocity set by user, moved by solver
		DYNAMIC		// positive mass, velocity determined by forces, moved by solver
	};

	enum class FilterCategory : std::uint16_t
	{
		// Default Types
		NO_CATEGORY = 0,
		PLAYER = 1 << 0,
		ENEMY = 1 << 1,
		ITEM = 1 << 2,
		WALLS = 1 << 3,
		GROUND = 1 << 4,
		TRIGGER = 1 << 5,
		PROJECTILE = 1 << 6,
		// User defined object types
		// These can have their visual name changed in the editor; however, in the BE, the original names and values are used.
		CATEGORY_7 = 1 << 7,
		CATEGORY_8 = 1 << 8,
		CATEGORY_9 = 1 << 9,
		CATEGORY_10 = 1 << 10,
		CATEGORY_11 = 1 << 11,
		CATEGORY_12 = 1 << 12,
		CATEGORY_13 = 1 << 13,
		CATEGORY_14 = 1 << 14,
		CATEGORY_15 = 1 << 15
	};

	std::string GetFilterCategoryString(FilterCategory eCat);

	FilterCategory GetFilterCategoryByString(const std::string& sCat);

	bool AddCustomFilterCategoryType(const std::string& sNewFilterCatType, std::string& sErrorStr);

	const std::map<FilterCategory, std::string> GetFilterCategoryToStringMap();

	std::string GetRigidBodyTypeString(RigidBodyType eRigidType);
	RigidBodyType GetRigidBodyTypeByString(const std::string sRigidType);
	const std::map<RigidBodyType, std::string>& GetRigidBodyStringMap();

} // namespace VORTEK_PHYSICS