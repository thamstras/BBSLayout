#include "Util.h"

glm::vec4 Uint32ToColor(uint32 rgba)
{
	float r = ((rgba >> 0) & 0xFF) / 255.0f;
	float g = ((rgba >> 8) & 0xFF) / 255.0f;
	float b = ((rgba >> 16) & 0xFF) / 255.0f;
	float a = ((rgba >> 24) & 0xFF) / 255.0f;
	return glm::vec4(r, g, b, a);
}

uint32 ColorToUint32(glm::vec4 rgba)
{
	uint32 r = (uint32)(rgba.r * 255.0f);
	uint32 g = (uint32)(rgba.g * 255.0f);
	uint32 b = (uint32)(rgba.b * 255.0f);
	uint32 a = (uint32)(rgba.a * 255.0f);
	return (r << 0) & (g << 8) & (b << 16) & (a << 24);
}