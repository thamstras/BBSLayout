#pragma once
#include "Common.hpp"
#include <cstdint>
#include "./PixelFormat.h"


class Texture
{
public:
	Texture(uint32_t width, uint32_t height, uint8_t *data, PixelFormat format, void* userPtr = nullptr);
	~Texture();

	void ogl_loadIfNeeded();
	void ogl_unload();

	uint32_t getWidth() const;
	uint32_t getHeight() const;
	const uint8_t* getPixels() const;

	GLuint getOglId() const;

	bool isLoaded() const;

	void* userPtr;

private:
	unsigned int width, height;
	uint8_t *data;
	GLuint ogl_texid;
	bool ogl_loaded;
};