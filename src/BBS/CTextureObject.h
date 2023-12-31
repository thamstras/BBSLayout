#pragma once
#include "Common.hpp"
#include "FileTypes\Tim2.h"
#include "Framework\Texture.h"

namespace BBS
{

	class CTextureObject
	{
	public:
		CTextureObject();
		~CTextureObject();

		void LoadTM2(Tm2File& tm2);
		void CreateTexture();

		Texture* texture;

		CLT_TYPE clutType;
		uint16 clutCount;
		std::vector<uint8> clut;

		IMG_TYPE imageType;
		uint16 imageWidth;
		uint16 imageHeight;
		std::vector<uint8> image;

		uint32 textureWidth;
		uint32 textureHeight;
	};

}