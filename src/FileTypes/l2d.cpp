#include "l2d.h"
#include <stdexcept>
#include "..\Utils\MagicCode.h"
#include "..\Utils\BitCast.h"
#include "..\Utils\StreamUtils.h"

constexpr uint32 SP2Signature = MagicCode('S', 'P', '2', '@');
constexpr uint32 SQ2Signature = MagicCode('S', 'Q', '2', '@');
constexpr uint32 SQ2PSignature = MagicCode('S', 'Q', '2', 'P');
constexpr uint32 LY2Signature = MagicCode('L', 'Y', '2', '@');
constexpr uint32 L2DSignature = MagicCode('L', '2', 'D', '@');

SP2File SP2File::ReadSP2File(std::istream& file)
{
	SP2File sp2;
	auto base = file.tellg();
	sp2.header = ReadStream<SP2Header>(file);
	if (sp2.header.Signature != SP2Signature)
	{
		throw new std::runtime_error("File is not a valid SP2 file! (Magic code fail)");
	}
	file.seekg(base + (std::streamoff)sp2.header.PartsOffset);
	for (int i = 0; i < sp2.header.PartsCount; i++) sp2.parts.push_back(ReadStream<SP2Part>(file));
	file.seekg(base + (std::streamoff)sp2.header.GroupOffset);
	for (int i = 0; i < sp2.header.GroupCount; i++) sp2.groups.push_back(ReadStream<SP2Group>(file));
	file.seekg(base + (std::streamoff)sp2.header.SpriteOffset);
	for (int i = 0; i < sp2.header.SpriteCount; i++) sp2.sprites.push_back(ReadStream<SP2Sprite>(file));
	return sp2;
}

SQ2File SQ2File::ReadSQ2File(std::istream& file)
{
	SQ2File sq2;
	auto base = file.tellg();
	sq2.header = ReadStream<SQ2Header>(file);
	if (sq2.header.Signature != SQ2Signature)
	{
		throw new std::runtime_error("File is not a valid SQ2 file! (Magic code fail)");
	}
	file.seekg(base + (std::streamoff)sq2.header.SequenceOffset);
	for (int i = 0; i < sq2.header.SequenceCount; i++) sq2.sequences.push_back(ReadStream<SQ2Sequence>(file));
	file.seekg(base + (std::streamoff)sq2.header.ControlOffset);
	for (int i = 0; i < sq2.header.ControlCount; i++) sq2.controls.push_back(ReadStream<SQ2Control>(file));
	file.seekg(base + (std::streamoff)sq2.header.AnimationOffset);
	for (int i = 0; i < sq2.header.AnimationCount; i++) sq2.animations.push_back(ReadStream<SQ2Animation>(file));
	file.seekg(base + (std::streamoff)sq2.header.KeyOffset);
	for (int i = 0; i < sq2.header.KeyCount; i++) sq2.keys.push_back(ReadStream<SQ2Key>(file));
	file.seekg(base + (std::streamoff)sq2.header.SequenceNameOffset);
	for (int i = 0; i < sq2.header.SequenceCount; i++) sq2.sequenceNames.push_back(ReadStream<Name16>(file));
	file.seekg(base + (std::streamoff)sq2.header.SequenceIDOffset);
	for (int i = 0; i < sq2.header.SequenceCount; i++) sq2.sequenceIDs.push_back(ReadStream<uint16>(file));
	return sq2;
}

SQ2PFile SQ2PFile::ReadSQ2PFile(std::istream& file)
{
	SQ2PFile sq2p;
	auto base = file.tellg();
	sq2p.header = ReadStream<SQ2PHeader>(file);
	if (sq2p.header.Signature != SQ2PSignature)
	{
		throw new std::runtime_error("File is not a valid SQ2P file! (Magic code fail)");
	}
	file.seekg(base + (std::streamoff)sq2p.header.SP2Offset);
	sq2p.sp2 = SP2File::ReadSP2File(file);
	file.seekg(base + (std::streamoff)sq2p.header.SQ2Offset);
	sq2p.sq2 = SQ2File::ReadSQ2File(file);
	file.seekg(base + (std::streamoff)sq2p.header.TM2Offset);
	sq2p.tm2 = Tm2File::ReadTm2File(file);
	return sq2p;
}

LY2File LY2File::ReadLY2File(std::istream& file)
{
	LY2File ly2;
	auto base = file.tellg();
	ly2.header = ReadStream<LY2Header>(file);
	if (ly2.header.Signature != LY2Signature)
	{
		throw new std::runtime_error("File is not a valid LY2 file! (Magic code fail)");
	}
	file.seekg(base + (std::streamoff)ly2.header.LayoutOffset);
	for (int i = 0; i < ly2.header.LayoutCount; i++) ly2.layouts.push_back(ReadStream<LY2Layout>(file));
	file.seekg(base + (std::streamoff)ly2.header.NodeOffset);
	for (int i = 0; i < ly2.header.NodeCount; i++) ly2.nodes.push_back(ReadStream<LY2Node>(file));
	file.seekg(base + (std::streamoff)ly2.header.FontInfoOffset);
	for (int i = 0; i < ly2.header.FontInfoCount; i++) ly2.fontInfos.push_back(ReadStream<LY2FontInfo>(file));
	file.seekg(base + (std::streamoff)ly2.header.StringOffset);
	for (int i = 0; i < ly2.header.StringCount; i++) ly2.strings.push_back(ReadCString(file));
	file.seekg(base + (std::streamoff)ly2.header.LayoutNameOffset);
	for (int i = 0; i < ly2.header.LayoutCount; i++) ly2.layoutNames.push_back(ReadStream<Name16>(file));
	file.seekg(base + (std::streamoff)ly2.header.LayoutIDOffset);
	for (int i = 0; i < ly2.header.LayoutCount; i++) ly2.layoutIDs.push_back(ReadStream<uint16>(file));
	return ly2;
}

L2DFile L2DFile::ReadL2DFile(std::istream& file)
{
	L2DFile l2d;
	auto base = file.tellg();
	l2d.header = ReadStream<L2DFileHeader>(file);
	if (l2d.header.Signature != L2DSignature)
	{
		throw new std::runtime_error("File is not a valid L2D file! (Magic code fail)");
	}
	// No, I don't know why +16 but it is.
	file.seekg(base + (std::streamoff)(l2d.header.SQ2POffset + 16));
	for (int i = 0; i < l2d.header.SQ2PCount; i++) l2d.sq2p.push_back(SQ2PFile::ReadSQ2PFile(file));
	file.seekg(base + (std::streamoff)l2d.header.LY2Offset);
	l2d.ly2 = LY2File::ReadLY2File(file);
	return l2d;
}