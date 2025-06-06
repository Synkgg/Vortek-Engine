#include "TextComponent.h"
#include <entt.hpp>

std::string VORTEK_CORE::ECS::TextComponent::to_string()
{
	std::stringstream ss;
	ss << "==== Text Component ==== \n"
		<< std::boolalpha << "Text: " << sTextStr << "\n"
		<< "Font Name: " << sFontName << "\n"
		<< "Padding: " << padding << "\n"
		<< "Wrap: " << wrap << "\n"
		<< "bHidden: " << bHidden << "\n"
		<< "Color: \n\t"
		<< "Red: " << color.r << "\n\t"
		<< "Green: " << color.g << "\n\t"
		<< "Blue: " << color.b << "\n\t"
		<< "Alpha: " << color.a << "\n";

	return ss.str();
}

void VORTEK_CORE::ECS::TextComponent::CreateLuaTextBindings(sol::state& lua)
{
	lua.new_usertype<TextComponent>(
		"TextComponent",
		"type_id",
		entt::type_hash<TextComponent>::value,
		sol::call_constructor,
		sol::factories(
			[](const std::string& sFontName,
				const std::string& sTextStr,
				VORTEK_RENDERING::Color color,
				int padding,
				float wrap) {
					return TextComponent{
						.sFontName = sFontName, .sTextStr = sTextStr, .padding = padding, .wrap = wrap, .color = color };
			},
			[](const std::string& sFontName, const std::string& sTextStr) {
				return TextComponent{ .sFontName = sFontName, .sTextStr = sTextStr };
			}),
		"textStr",
		&TextComponent::sTextStr,
		"fontName",
		&TextComponent::sFontName,
		"padding",
		&TextComponent::padding,
		"bHidden",
		&TextComponent::bHidden,
		"wrap",
		&TextComponent::wrap,
		"color",
		&TextComponent::color,
		"to_string",
		&TextComponent::to_string);
}