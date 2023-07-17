#include "Tim2.h"
#include <stdexcept>
#include "..\Utils\MagicCode.h"
#include "..\Utils\BitCast.h"
#include "..\Utils\StreamUtils.h"

constexpr uint32_t tim2Magic = MagicCode('T', 'I', 'M', '2');

Tm2FileHeader ParseTm2FileHeader(std::istream& file)
{
	Tm2FileHeader header;
	header = ReadStream<Tm2FileHeader>(file);
	//file >> header.magic;
	if (header.magic != tim2Magic)
	{
		throw new std::runtime_error("File is not a valid TM2 file! (Magic code fail)");
	}
	//file >> header.version >> header.format >> header.pictureCount;
	//file >> header.padding[0] >> header.padding[1];
	return header;
}

GsTex0 ParseGSTex(std::istream& file)
{
	uint64_t raw;
	GsTex0 reg;
	raw = ReadStream<uint64>(file);
	reg = bit_cast<GsTex0, uint64_t>(raw);
	return reg;
}

Tm2PictureHeader ParseTm2PictureHeader(std::istream& file)
{
	Tm2PictureHeader header;
	header = ReadStream<Tm2PictureHeader>(file);
	//file >> header.totalSize >> header.clutSize >> header.imageSize >> header.headerSize;
	//file >> header.clutColors >> header.format >> header.mipMapCount;
	//file >> header.clutType >> header.imageType >> header.width >> header.headerSize;
	//header.gsTex0b = ParseGSTex(file);
	//header.gsTex1b = ParseGSTex(file);
	//file >> header.gsRegs >> header.gsTexClut;
	return header;
}

Tm2Mipmap ParseTm2MipMap(std::istream& file)
{
	Tm2Mipmap mipmap;
	mipmap = ReadStream<Tm2Mipmap>(file);
	//file >> mipmap.miptbp0 >> mipmap.miptbp1 >> mipmap.miptbp2 >> mipmap.miptbp3;
	//for (int i = 0; i < 8; i++) file >> mipmap.sizes[i];
	return mipmap;
}

Tm2Picture ParseTm2Picture(std::istream& file)
{
	Tm2Picture picture;
	picture.header = ParseTm2PictureHeader(file);
	if (picture.header.mipMapCount > 1)
		picture.mipmap = ParseTm2MipMap(file);
	picture.pixelData = ReadBlob(file, picture.header.imageSize);
	picture.clutData = ReadBlob(file, 4 * picture.header.clutColors);
	return picture;
}

Tm2File Tm2File::ReadTm2File(std::istream& file)
{
	Tm2File tm2;
	tm2.header = ParseTm2FileHeader(file);
	for (int p = 0; p < tm2.header.pictureCount; p++)
		tm2.pictures.push_back(ParseTm2Picture(file));
	return tm2;
}

uint16 Tm2File::pictureCount()
{
	return header.pictureCount;
}