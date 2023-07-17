#include "Framework/Image.h"
#include "Framework/Texture.h"
#include <vector>
#include "FileTypes/l2d.h"

struct SpritePart;
struct SpriteGroup;
struct Sprite;

class SpriteSet
{
private:
	Image image;
	Texture texture;
	std::vector<SpritePart> spriteParts;
	std::vector<SpriteGroup> groups;
	std::vector<Sprite> sprites;

public:
	static SpriteSet FromSP2(SP2File sp2);

	void Use();
	void DrawSprite(int id, int x, int y);
};