#pragma once
#include "FileTypes/l2d.h"
#include <string>
#include "Framework/Texture.h"
#include "Framework/BTypes.h"
#include "Framework/Shader.h"
#include "Framework/ShaderLibrary.h"
#include "BBS/CTextureObject.h"

class Layout;

extern Layout* g_layout;

struct SprRect
{
	float U0, V0, U1, V1;
	uint32 RGBA0, RGBA1, RGBA2, RGBA3;
};

struct SprPart
{
	SprRect rect;
	int16 X0, Y0, X1, Y1;
};

struct Sprite
{
	std::vector<SprPart> parts;
};

struct SpriteSet
{
	std::vector<Sprite> sprites;
	Texture* texture;
};

class SpriteRenderer
{
public:
	GLuint VBO, VAO;
	std::shared_ptr<Shader> shader;
	void Setup(std::shared_ptr<Shader> shader);

	void DrawSprite(Texture* tex, glm::vec2 pos0, glm::vec2 pos1, glm::vec2 uv0, glm::vec2 uv1,
		glm::vec4 c0, glm::vec4 c1, glm::vec4 c2, glm::vec4 c3);
};

enum eInterpolateType
{
	INTERP_LINEAR,
	INTERP_SPLINE,
	INTERP_SWAY,
	INTERP_POINT
};

class IntKey
{
public:
	float time;
	int value;
	uint8 exta[4];
};

class IntCurve
{
public:
	eInterpolateType interpolateType;
	int keyCount;
	std::vector<IntKey> keyVals;
	int defaultVal;

	int evaluate(float time);
};

class FloatKey
{
public:
	float time;
	float value;
	uint8 extra[4];
};

class FloatCurve
{
public:
	eInterpolateType interpolateType;
	int keyCount;
	std::vector<FloatKey> keyVals;
	float defaultVal;

	float evaluate(float time);
};

class CurveSet
{
public:
	IntCurve statusCurve;
	FloatCurve baseXCurve;
	FloatCurve baseYCurve;
	FloatCurve offsetXCurve;
	FloatCurve offsetYCurve;
	FloatCurve rotateXCurve;
	FloatCurve rotateYCurve;
	FloatCurve rotateZCurve;
	FloatCurve scaleXCurve;
	FloatCurve scaleYCurve;
	IntCurve colorCurve;
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
		05 ROTATE X	(???)	???
		06 ROTATE Y	(???)	???
		07 ROTATE Z (???)	???
		08 SCALE X			Probably FLOAT
		09 SCALE Y			Probably FLOAT
		10 COLOR			INT

		extra data:
			First byte is Type.
			For Spline (0x40) the remaining 3 bytes are the midpoint control somehow.

	(No, I'm not sure how you can have 3 rotations in a 2d system either)
*/

class SequenceAnimation
{
public:
	int32 maxFrameNumber;
	int16 spriteNumber;
	int16 keyStartOffset;
	char keyData[11];
	uint8 kind;
	uint8 blendType;
	bool bDitherOff;
	bool bBilinear;
	uint8 scissorNumber;
	uint8 zDepth;

	CurveSet animCurves;
};

class SequenceChannel
{
public:
	int32 maxFrame;
	int32 returnFrame;
	int16 loopNumber;
	std::vector<SequenceAnimation> animations;
};

class Sequence
{
public:
	uint16 ID;
	std::string name;
	std::vector<SequenceChannel> controls;
	int16 layer;
};

class SequenceSet
{
public:
	std::vector<Sequence> sequences;
	std::vector<SQ2Key> keys;
	size_t spriteSetID;
};

class Layout
{
public:
	Layout();
	~Layout();
	
	void Setup(std::shared_ptr<ShaderLibrary> shaderLibrary);

	bool LoadL2D(std::string filepath);
	Texture* TextureFromTM2(Tm2File& tm2);

	void Tick(float deltaTime, double worldTime);
	void Render();

	void DrawSingleSprite(int setIdx, int spriteIdx);
	void DrawSequenceFrame(int setID, int sequenceID, int channelID, int frame);

	int GetSpriteSetCount();
	int GetSpriteCount(int setIdx);

	void gui_SequencesList();
	void gui_KeysList();

private:
	L2DFile loadedFile;
	std::vector<BBS::CTextureObject*> texObjects;
	std::vector<Texture*> textures;
	std::vector<SpriteSet> spriteSets;
	std::vector<SequenceSet> sequenceSets;
	SpriteRenderer spriteRenderer;

};