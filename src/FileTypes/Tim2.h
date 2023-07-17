#pragma once
#include <vector>
#include <fstream>
#include "Framework/BTypes.h"

enum GsPSM
{
	// Pixel Storage Format (0 = 32bit RGBA)

	GS_PSMCT32 = 0,
	GS_PSMCT24 = 1,
	GS_PSMCT16 = 2,
	GS_PSMCT16S = 10,
	GS_PSMT8 = 19,
	GS_PSMT4 = 20,
	GS_PSMT8H = 27,
	GS_PSMT4HL = 36,
	GS_PSMT4HH = 44,
	GS_PSMZ32 = 48,
	GS_PSMZ24 = 49,
	GS_PSMZ16 = 50,
	GS_PSMZ16S = 58,
};

enum IMG_TYPE
{
	IT_A1BGR5 = 1,
	IT_XBGR8 = 2,
	IT_RGBA = 3,
	IT_CLUT4 = 4,
	IT_CLUT8 = 5,
};

enum CLT_TYPE
{
	CT_NONE = 0,
	CT_A1BGR5 = 1,
	CT_XBGR8 = 2,
	CT_ABGR8 = 3,
};

struct GsTex0
{
	uint64 TBP0 : 14; // Texture Buffer Base Pointer (Address/256)
	uint64 TBW : 6; // Texture Buffer Width (Texels/64)
	uint64 PSM : 6; // Pixel Storage Format (0 = 32bit RGBA)
	uint64 TW : 4; // width = 2^TW
	uint64 TH : 4; // height = 2^TH
	uint64 TCC : 1; // 0 = RGB, 1 = RGBA
	uint64 TFX : 2; // TFX  - Texture Function (0=modulate, 1=decal, 2=hilight, 3=hilight2)
	uint64 CBP : 14; // CLUT Buffer Base Pointer (Address/256)
	uint64 CPSM : 4; // CLUT Storage Format
	uint64 CSM : 1; // CLUT Storage Mode
	uint64 CSA : 5; // CLUT Offset
	uint64 CLD : 3; // CLUT Load Control
};

struct Tm2FileHeader
{
	uint32 magic;
	uint8 version;
	uint8 format;
	uint16 pictureCount;
	uint32 padding[2];
};

struct Tm2PictureHeader
{
	uint32 totalSize;
	uint32 clutSize;
	uint32 imageSize;
	uint16 headerSize;
	uint16 clutColors;
	uint8  format;
	uint8  mipMapCount;
	uint8  clutType;
	uint8  imageType;
	uint16 width;
	uint16 height;
	GsTex0 gsTex0b;
	GsTex0 gsTex1b;
	uint32 gsRegs;
	uint32 gsTexClut;
};

struct Tm2Mipmap
{
	uint32 miptbp0;
	uint32 miptbp1;
	uint32 miptbp2;
	uint32 miptbp3;
	uint32 sizes[8];
};

struct Tm2Picture
{
	Tm2PictureHeader header;
	Tm2Mipmap mipmap;
	std::vector<uint8> pixelData;
	std::vector<uint8> clutData;
};

class Tm2File
{
public:
	Tm2FileHeader header;
	std::vector<Tm2Picture> pictures;

	uint16 pictureCount();

	static Tm2File ReadTm2File(std::istream& file);
};