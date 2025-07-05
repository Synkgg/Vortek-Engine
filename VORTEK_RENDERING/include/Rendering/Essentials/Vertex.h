#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <sstream>

namespace VORTEK_RENDERING
{

struct Color
{
	GLubyte r{ 255 };
	GLubyte g{ 255 };
	GLubyte b{ 255 };
	GLubyte a{ 255 };
};

struct Vertex
{
	glm::vec2 position{ 0.f };
	glm::vec2 uvs{ 0.f };
	Color color{ .r = 255, .g = 255, .b = 255, .a = 255 };

	void set_color( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
	{
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}

	/*
	 * @brief Takes in a GLuint and then parses the values into the
	 * rgba values of color. It does this using the bitshift operator.
	 * This function allows us to easily create constant colors to send in.
	 * @param GLuint as the desired color -- 0xFF0000FF == RED
	 */
	void set_color( GLuint newColor )
	{
		color.r = ( newColor >> 24 ) & 0xFF;
		color.g = ( newColor >> 16 ) & 0xFF;
		color.b = ( newColor >> 8 ) & 0xFF;
		color.a = ( newColor >> 0 ) & 0xFF;
	}

	[[nodiscard]] std::string to_string() const
	{
		std::stringstream ss;
		ss << "==== Color ==== \n"
		   << "Red: " << color.r << "\n"
		   << "Green: " << color.g << "\n"
		   << "Blue: " << color.b << "\n"
		   << "Alpha: " << color.a << "\n";

		return ss.str();
	}
};

struct CircleVertex
{
	glm::vec2 position;
	glm::vec2 uvs;
	Color color;
	float lineThickness;
};

struct PickingVertex
{
	glm::vec2 position{ 0.f };
	glm::vec2 uvs{ 0.f };
	Color color{};
	uint32_t uid{ 0 };
};

} // namespace VORTEK_RENDERING
