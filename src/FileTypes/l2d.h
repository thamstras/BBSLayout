#pragma once
#include <vector>
#include <fstream>
#include "Framework/BTypes.h"
#include "Tim2.h"

struct L2DFileHeader
{
    //char Signature[4];
    uint32 Signature;
    char Version[4];
    char Date[8];
    char Name[4];
    uint8 Reserved1[4];
    uint8 Reserved2[8];
    int32 SQ2PCount;
    int32 SQ2POffset;
    int32 LY2Offset;
    int32 FileSize;
    uint8 Reserved3[16];
};

struct SQ2PHeader
{
    //char Signature[4];
    uint32 Signature;
    char Version[4];
    uint8 Reserved1[8];
    uint32 SP2Offset;
    uint32 SQ2Offset;
    uint32 TM2Offset;
    uint8 Reserved2[36];
};

struct SP2Header
{
    //char Signature[4];
    uint32 Signature;
    char Version[4];
    uint8 Reserved1[8];
    int32 PartsCount;
    int32 PartsOffset;
    int32 GroupCount;
    int32 GroupOffset;
    int32 SpriteCount;
    int32 SpriteOffset;
    int8 Reserved2[24];
};

/*
    SP2 PART NOTES

    RGBA0          RGBA1
    U0V0           U1V0
        *-----------*
        |          /|
        |        /  |
        |      /    |
        |    /      |
        |  /        |
        |/          |
        *-----------*
    U0V1            U1V1
    RGBA2           RGBA3

*/

// Defines a clip rect
struct SP2Part
{
    int16 U0, V0, U1, V1;               // Corners of clip rect in pixels
    uint32 RGBA0, RGBA1, RGBA2, RGBA3;  // vertex tint
};

// Positions a clip rect?
struct SP2Group
{
    int16 X0, Y0, X1, Y1;       // rel pos?
    uint16 IDXParts;            // part id?
    uint16 Attribute;
};

// Groups groups into a sprite
struct SP2Sprite
{
    uint16 GroupValue;  // group count
    uint16 GroupIDX;    // offset of first group in the group array
};

class SP2File
{
public:
    SP2Header header;
    std::vector<SP2Part> parts;
    std::vector<SP2Group> groups;
    std::vector<SP2Sprite> sprites;

    static SP2File ReadSP2File(std::istream& file);
};

struct SQ2Header
{
    //char Signature[4];
    uint32 Signature;
    char Version[4];
    uint8 Reserved1[8];
    int32 SequenceCount;
    int32 SequenceOffset;
    int32 ControlCount;
    int32 ControlOffset;
    int32 AnimationCount;
    int32 AnimationOffset;
    int32 KeyCount;
    int32 KeyOffset;
    int32 SequenceNameOffset;
    int32 SequenceIDOffset;
    int8 Reserved2[8];
};

struct SQ2Sequence
{
    int32 ControlIDX;
    int16 ControlNumber;
    int16 LayerNumber;
};

struct SQ2Control
{
    int32 MaxFrame;
    int32 ReturnFrame;
    int32 AnimationIDX;
    int16 LoopNumber;
    int16 AnimationNumber;
};

/*
    KEY DATA NOTES
    Each byte of the keyData array contains how many keys for that byte's corresponding attribute to read
    Attribs:
        00 STATUS			???
        01 BASE X			FLOAT
        02 BASE Y			FLOAT
        03 OFFSET X			FLOAT			(Has extra info in key's Data)
        04 OFFSET Y			FLOAT			(Has extra info in key's Data)
        05 ROTATE X			???
        06 ROTATE Y			???
        07 ROTATE Z (???)	???
        08 SCALE X			Probably FLOAT
        09 SCALE Y			Probably FLOAT
        10 COLOR			INT
*/

struct SQ2Animation
{
    int32 MaxFrameNumber;
    int16 SpriteNumber;
    int16 NOfsKeyData;      // Offset into SQ2 File's keys array.
    char KeyDataArray[11];
    uint8 Kind;
    uint8 BlendType;
    struct
    {
        uint8 dummy : 6;
        uint8 bDitherOff : 1;
        uint8 bBilinear : 1;
    };
    uint8 ScissorNumber;
    uint8 ZDepth;
};

struct SQ2Key
{
    float Key;
    union
    {
        int32 iValue;
        float fValue;
    };
    uint8 Data[4];
};

// TODO: maybe std::array<char, 16> would be better. We could even dump them straight into std::strings from there.
// IE: sq2.sequenceNames.push_back(std::string(ReadStream<std::array<char, 16>>(file));
// Would need to take care of missing null terminators. All names I've seen so far have them but better to be safe than sorry.
// TODO: std::string(char*, size_t) iirc.
struct Name16
{
    char name[16];
};

class SQ2File
{
public:
    SQ2Header header;
    std::vector<SQ2Sequence> sequences;
    std::vector<SQ2Control> controls;
    std::vector<SQ2Animation> animations;
    std::vector<SQ2Key> keys;
    std::vector<Name16> sequenceNames;
    std::vector<uint16> sequenceIDs;

    static SQ2File ReadSQ2File(std::istream& file);
};

class SQ2PFile
{
public:
    SQ2PHeader header;
    SP2File sp2;
    SQ2File sq2;
    Tm2File tm2;

    static SQ2PFile ReadSQ2PFile(std::istream& file);
};

struct LY2Header
{
    //char Signature[4];
    uint32 Signature;
    char Version[4];
    uint8 Reserved[8];
    int32 LayoutCount;
    int32 LayoutOffset;
    int32 ControlCount;
    int32 ControlOffset;
    int32 NodeCount;
    int32 NodeOffset;
    int32 FontInfoCount;
    int32 FontInfoOffset;
    int32 StringCount;
    int32 StringOffset;
    int32 LayoutNameOffset;
    int32 LayoutIDOffset;
};

struct LY2Layout
{
    int32 MaxFrame;
    int32 NodeIDX;
    int16 Node;
    int16 Layer;
    int16 X;
    int16 Y;
};

struct LY2Node
{
    int32 MaxFrame;
    int16 SQ2BaseNumber;
    int16 SequenceIDX;
    int8 AffectTranslation;
    int8 AffectColor;
    int8 Reserved1[2];
    int32 FontInfo;
    int32 FontInfoIDX;
    int16 ParentIDX;
    int16 X;
    int16 Y;
    int16 ID;
    int32 Reserved2;
};

struct LY2FontInfo
{
    int32 StringIDX;
    int32 FontColor;
    int8 FontSize;
    int8 FontKind;
    int8 FontCenter;
    int8 FontType;
    int32 Reserved;
};

class LY2File
{
public:
    LY2Header header;
    std::vector<LY2Layout> layouts;
    // error: LY2_CONTROL not found...
    std::vector<LY2Node> nodes;
    std::vector<LY2FontInfo> fontInfos;
    std::vector<std::string> strings;
    std::vector<Name16> layoutNames;
    std::vector<uint16> layoutIDs;

    static LY2File ReadLY2File(std::istream& file);
};

class L2DFile
{
public:
    L2DFileHeader header;
    std::vector<SQ2PFile> sq2p;
    LY2File ly2;

    static L2DFile ReadL2DFile(std::istream& file);
};