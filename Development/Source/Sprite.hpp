#pragma once
namespace PinningMode
{
	enum
	{
		TopLeft = 0, //Default
		TopCenter,
		TopRight,
		Left,
		Middle,
		Right,
		BottomLeft,
		BottomCenter,
		BottomRight
	};
};

namespace CropMode
{
	enum
	{
		None = 0, //Default
		Crop,
		CropNormalized, //[Normalized Coordinates]
		CropTiled, //[Right/Bottom -> TileID, Left/Top -> FrameSize]
	};
};

namespace BlendingMode
{
	enum
	{
		None = 0,
		Alpha,
		Additive,
		Subtractive
	};
};

class SpriteDrawOptions
{
public:
	Vector2 PositionValue, ScaleValue, OffsetValue;
	float RotationValue;
	unsigned long PinningModeValue;
	unsigned long CropModeValue;
	Rect CropRectValue;
	Vector4 ColorValue;
	unsigned long BlendingModeValue;
	bool NinePatchValue;
	Rect NinePatchRectValue;

	SpriteDrawOptions() : ColorValue(1, 1, 1, 1), ScaleValue(1, 1), RotationValue(0), BlendingModeValue(BlendingMode::Alpha),
		PinningModeValue(PinningMode::TopLeft), CropModeValue(CropMode::None), NinePatchValue(false) {};
	SpriteDrawOptions(const SpriteDrawOptions &o) : ColorValue(o.ColorValue), BlendingModeValue(o.BlendingModeValue),
		PositionValue(o.PositionValue), ScaleValue(o.ScaleValue), RotationValue(o.RotationValue),
		PinningModeValue(o.PinningModeValue), CropModeValue(o.CropModeValue), CropRectValue(o.CropRectValue),
		NinePatchValue(o.NinePatchValue), NinePatchRectValue(o.NinePatchRectValue) {};

	SpriteDrawOptions &Position(const Vector2 &Pos) { PositionValue = Pos; return *this; };
	//Scaling specifies the object's size when NinePatching
	SpriteDrawOptions &Scale(const Vector2 &Scale) { ScaleValue = Scale; return *this; };
	SpriteDrawOptions &Color(const Vector4 &Color) { ColorValue = Color; return *this; };
	SpriteDrawOptions &BlendingMode(unsigned long Blending) { BlendingModeValue = Blending; return *this; };
	SpriteDrawOptions &Rotation(float Rotation) { RotationValue = Rotation; return *this; };
	SpriteDrawOptions &Pin(unsigned long PinningMode) { PinningModeValue = PinningMode; return *this; };
	SpriteDrawOptions &Offset(const Vector2 &Offset) { OffsetValue = Offset; return *this; };

	//Ignored on Nine Patches
	SpriteDrawOptions &Crop(unsigned long CropMode, const Rect &CropRect)
	{
		CropModeValue = CropMode;
		CropRectValue = CropRect;
	
		return *this;
	};

	SpriteDrawOptions &NinePatch(bool NinePatch, const Rect &NinePatchRect)
	{
		NinePatchValue = NinePatch;
		NinePatchRectValue = NinePatchRect;

		return *this;
	};
};

class Sprite
{
public:
	SuperSmartPointer<sf::Texture> SpriteTexture;
	SpriteDrawOptions Options;

	void Draw(sf::RenderWindow *Renderer);
};

class AnimatedSprite : public Sprite
{
	struct AnimationInfo
	{
		std::vector<Vector2> Frames;
		bool Repeating;
		unsigned long CurrentFrame;

		AnimationInfo() : Repeating(true), CurrentFrame(0) {};
	};

	typedef std::map<StringID, AnimationInfo> FrameMap;
	FrameMap Animations;
	unsigned long long LastFrameUpdate;
	AnimationInfo *CurrentAnimation;
public:
	Vector2 FrameSize, DefaultFrame, Scale;

	unsigned long long FrameInterval;

	AnimatedSprite(const Vector2 &SpriteFrameSize, const Vector2 &SpriteDefaultFrame = Vector2()) :
		FrameSize(SpriteFrameSize), LastFrameUpdate(0), CurrentAnimation(NULL), DefaultFrame(SpriteDefaultFrame),
		FrameInterval(250), Scale(1, 1) {};

	void AddAnimation(const std::string &Name, const std::vector<Vector2> &Frames);
	//May pass any name if wishing to set no animation
	void SetAnimation(const std::string &Name, bool Repeats);
	void StopAnimation();

	void Update();
};
