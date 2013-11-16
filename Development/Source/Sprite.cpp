#include <map>
#include <vector>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <json/json.h>
#include <math.h>
#include "SuperSmartPointer.hpp"
#include "StringID.hpp"
#include "Math.hpp"
#include "Sprite.hpp"

//Generates a ninepatch quad
void GenerateNinePatchGeometry(Vector2 *Vertices, Vector2 *TexCoords, const Vector2 &TextureSize, const Vector2 &Position, const Vector2 &Size, const Vector2 &Offset,
							   const Vector2 &SizeOverride = Vector2(-1, -1))
{
	static Rect NinePatchRect;

	NinePatchRect = Rect(Position.x, Position.x + Size.x, Position.y, Position.y + Size.y);

	Vector2 ActualSize = SizeOverride.x != -1 ? SizeOverride : Size;

	Vertices[0] = Vertices[5] = Offset;
	Vertices[1] = Offset + Vector2(0, ActualSize.y);
	Vertices[2] = Vertices[3] = Offset + ActualSize;
	Vertices[4] = Offset + Vector2(ActualSize.x, 0);

	TexCoords[0] = TexCoords[5] = NinePatchRect.Position() / TextureSize;
	TexCoords[1] = Vector2(NinePatchRect.Left, NinePatchRect.Bottom) / TextureSize;
	TexCoords[2] = TexCoords[3] = Vector2(NinePatchRect.Right, NinePatchRect.Bottom) / TextureSize;
	TexCoords[4] = Vector2(NinePatchRect.Right, NinePatchRect.Top) / TextureSize;
};

void Sprite::Draw(sf::RenderWindow *Renderer)
{
	static unsigned long LastBlendingMode = BlendingMode::None;

	if(Options.BlendingModeValue != LastBlendingMode)
	{
		switch(Options.BlendingModeValue)
		{
		case BlendingMode::None:
			glDisable(GL_BLEND);

			break;
		case BlendingMode::Alpha:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			break;
		case BlendingMode::Additive:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			break;
		case BlendingMode::Subtractive:
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_SUBTRACT);

			break;
		};
	};

	Vector2 Vertices[6] = {
		Vector2(),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 1),
		Vector2(1, 0),
		Vector2()
	},
	TexCoords[6] = {
		Vector2(),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 1),
		Vector2(1, 0),
		Vector2()
	};

		Vector4 Colors[6] = {
			Options.ColorValue,
			Options.ColorValue,
			Options.ColorValue,
			Options.ColorValue,
			Options.ColorValue,
			Options.ColorValue,
		};

		Vector2 NinePatchVertices[54], NinePatchTexCoords[54];
		Vector4 NinePatchColors[54] = {
			Options.ColorValue
		};

		Vector2 *VerticesTarget = Options.NinePatchValue ? NinePatchVertices : Vertices, *TexCoordTarget = Options.NinePatchValue ? NinePatchTexCoords : TexCoords;
		Vector4 *ColorsTarget = Options.NinePatchValue ? NinePatchColors : Colors;

		unsigned long VertexCount = Options.NinePatchValue ? 54 : 6;

		if(!Options.NinePatchValue)
		{
			if(SpriteTexture.Get())
			{
				for(unsigned long i = 0; i < VertexCount; i++)
				{
					VerticesTarget[i] *= Vector2(SpriteTexture->getSize().x, SpriteTexture->getSize().y);
					VerticesTarget[i] *= Options.ScaleValue;
				};
			}
			else
			{
				for(unsigned long i = 0; i < VertexCount; i++)
				{
					VerticesTarget[i] *= Options.ScaleValue;
				};
			};
		}
		else
		{
			const Rect &TextureRect = Options.NinePatchRectValue;

			Vector2 FragmentPositions[9] = {
				Vector2(),
				Vector2(SpriteTexture->getSize().x - TextureRect.Right, 0),
				Vector2(0, SpriteTexture->getSize().y - TextureRect.Bottom),
				Vector2(SpriteTexture->getSize().x - TextureRect.Right, SpriteTexture->getSize().y - TextureRect.Bottom),
				Vector2(TextureRect.Left, TextureRect.Top),
				Vector2(TextureRect.Left, 0),
				Vector2(TextureRect.Left, SpriteTexture->getSize().y - TextureRect.Bottom),
				Vector2(0, TextureRect.Top),
				Vector2(SpriteTexture->getSize().x - TextureRect.Right, TextureRect.Top),
			};

			Vector2 FragmentSizes[9] = {
				Vector2(TextureRect.Left, TextureRect.Top),
				Vector2(TextureRect.Right, TextureRect.Top),
				Vector2(TextureRect.Left, TextureRect.Bottom),
				Vector2(TextureRect.Right, TextureRect.Bottom),
				Vector2(SpriteTexture->getSize().x - TextureRect.Left - TextureRect.Right, SpriteTexture->getSize().y -
				TextureRect.Top - TextureRect.Bottom),
				Vector2(SpriteTexture->getSize().x - TextureRect.Left - TextureRect.Right, TextureRect.Top),
				Vector2(SpriteTexture->getSize().x - TextureRect.Left - TextureRect.Right, TextureRect.Bottom),
				Vector2(TextureRect.Left, SpriteTexture->getSize().y - TextureRect.Top - TextureRect.Bottom),
				Vector2(TextureRect.Right, SpriteTexture->getSize().y - TextureRect.Top - TextureRect.Bottom),
			};

			Vector2 FragmentOffsets[9] = {
				Vector2(-TextureRect.Left, -TextureRect.Top),
				Vector2(MathUtils::Round(Options.ScaleValue.x), -TextureRect.Top),
				Vector2(-TextureRect.Left, MathUtils::Round(Options.ScaleValue.y)),
				Vector2(MathUtils::Round(Options.ScaleValue.x), MathUtils::Round(Options.ScaleValue.y)),
				Vector2(),
				Vector2(0, -TextureRect.Top),
				Vector2(0, MathUtils::Round(Options.ScaleValue.y)),
				Vector2(-TextureRect.Left, 0),
				Vector2(MathUtils::Round(Options.ScaleValue.x), 0),
			};

			Vector2 FragmentSizeOverrides[9] = {
				Vector2(-1, -1),
				Vector2(-1, -1),
				Vector2(-1, -1),
				Vector2(-1, -1),
				Vector2(MathUtils::Round(Options.ScaleValue.x), MathUtils::Round(Options.ScaleValue.y)),
				Vector2(MathUtils::Round(Options.ScaleValue.x), TextureRect.Top),
				Vector2(MathUtils::Round(Options.ScaleValue.x), TextureRect.Bottom),
				Vector2(TextureRect.Left, MathUtils::Round(Options.ScaleValue.y)),
				Vector2(TextureRect.Right, MathUtils::Round(Options.ScaleValue.y)),
			};

			for(unsigned long i = 0, index = 0; i < 9; i++, index += 6)
			{
				GenerateNinePatchGeometry(VerticesTarget + index, TexCoordTarget + index, Vector2(SpriteTexture->getSize().x,
					SpriteTexture->getSize().y), FragmentPositions[i], FragmentSizes[i], FragmentOffsets[i], FragmentSizeOverrides[i]);
			};
		};

		Vector2 PinningTranslation;

		Vector2 ObjectSize = Vector2();

		for(unsigned long i = 0; i < VertexCount; i++)
		{
			Vector2 Difference = VerticesTarget[i] - (Options.PositionValue + Options.OffsetValue);

			if(Difference.x > ObjectSize.x)
				ObjectSize.x = Difference.x;

			if(Difference.y > ObjectSize.y)
				ObjectSize.y = Difference.y;
		};

		switch(Options.PinningModeValue)
		{
		case PinningMode::TopLeft:
			//Ignore

			break;
		case PinningMode::TopCenter:
			PinningTranslation = Vector2(ObjectSize.x / 2, 0);

			break;
		case PinningMode::TopRight:
			PinningTranslation = Vector2(ObjectSize.x, 0);

			break;
		case PinningMode::Left:
			PinningTranslation = Vector2(0, ObjectSize.y / 2);

			break;
		case PinningMode::Middle:
			PinningTranslation = Vector2(ObjectSize.x / 2, ObjectSize.y / 2);

			break;
		case PinningMode::Right:
			PinningTranslation = Vector2(ObjectSize.x, ObjectSize.y / 2);

			break;
		case PinningMode::BottomLeft:
			PinningTranslation = Vector2(0, ObjectSize.y);

			break;
		case PinningMode::BottomCenter:
			PinningTranslation = Vector2(ObjectSize.x / 2, ObjectSize.y);

			break;
		case PinningMode::BottomRight:
			PinningTranslation = Vector2(ObjectSize.x, ObjectSize.y);

			break;
		};

		if(Options.RotationValue != 0)
		{
			for(unsigned long i = 0; i < VertexCount; i++)
			{
				VerticesTarget[i] = Vector2::Rotate(VerticesTarget[i] - ObjectSize / 2, Options.RotationValue);
				VerticesTarget[i] += PinningTranslation + Options.OffsetValue + Options.PositionValue;
			};
		}
		else
		{
			for(unsigned long i = 0; i < VertexCount; i++)
			{
				VerticesTarget[i] += PinningTranslation + Options.OffsetValue + Options.PositionValue;
			};
		}

		if(!Options.NinePatchValue && Options.CropModeValue != CropMode::None)
		{
			switch(Options.CropModeValue)
			{
			case CropMode::Crop:
				//1st: Normalize
				{
					Vector2 NormalizedSize = Options.CropRectValue.Size() / Vector2(SpriteTexture->getSize().x, SpriteTexture->getSize().y);
					Vector2 NormalizedPosition = Options.CropRectValue.Position() / Vector2(SpriteTexture->getSize().x, SpriteTexture->getSize().y);

					//2nd: Multiply by size, then translate
					for(unsigned long i = 0; i < VertexCount; i++)
					{
						TexCoordTarget[i] *= NormalizedSize;
						TexCoordTarget[i] += NormalizedPosition;
					};
				};

				break;
			case CropMode::CropNormalized:
				for(unsigned long i = 0; i < VertexCount; i++)
				{
					TexCoordTarget[i] *= Options.CropRectValue.Size();
					TexCoordTarget[i] += Options.CropRectValue.Position();
				};

				break;
			case CropMode::CropTiled:
				{
					Vector2 OneFrame = Options.CropRectValue.Position() / Vector2(SpriteTexture->getSize().x, SpriteTexture->getSize().y);
					Vector2 BaseFrame = OneFrame * Vector2(Options.CropRectValue.Right, Options.CropRectValue.Bottom);

					for(unsigned long i = 0; i < VertexCount; i++)
					{
						TexCoordTarget[i] *= OneFrame;
						TexCoordTarget[i] += BaseFrame;
					};
				};

				break;
			};
		};

		sf::Texture::bind(SpriteTexture.Get());
		glDisableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, VerticesTarget);

		if(SpriteTexture.Get() == NULL)
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, TexCoordTarget);
		};

		if(Options.ColorValue != Vector4(1, 1, 1, 1))
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, 0, ColorsTarget);
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY);
			glColor4f(1, 1, 1, 1);
		};

		glDrawArrays(GL_TRIANGLES, 0, VertexCount);

		glColor4f(1, 1, 1, 1);
};

void AnimatedSprite::AddAnimation(const std::string &Name, const std::vector<Vector2> &Frames)
{
	StringID NameID = MakeStringID(Name);

	if(Animations.find(NameID) != Animations.end())
		return;

	Animations[NameID].Frames = Frames;
};

void AnimatedSprite::SetAnimation(const std::string &Name, bool Repeats)
{
	StringID NameID = MakeStringID(Name);

	if(Animations.find(NameID) == Animations.end())
	{
		CurrentAnimation = NULL;
		LastFrameUpdate = 0;

		return;
	};

	CurrentAnimation = &Animations[NameID];
	CurrentAnimation->Repeating = Repeats;
	LastFrameUpdate = 0;
};

void AnimatedSprite::Update()
{
	if(LastFrameUpdate == 0)
	{
		LastFrameUpdate = time(NULL);

		if(CurrentAnimation != NULL && CurrentAnimation->Frames.size())
		{
			CurrentAnimation->CurrentFrame = 0;
		};

		Vector2 Frame = CurrentAnimation != NULL && CurrentAnimation->Frames.size() ?
			CurrentAnimation->Frames[CurrentAnimation->CurrentFrame] : DefaultFrame;

		Options = Options.Crop(CropMode::CropTiled, Rect(FrameSize.x, Frame.x, FrameSize.y, Frame.y));

		if(SpriteTexture.Get())
		{
			Options = Options.Scale(FrameSize / Vector2(SpriteTexture->getSize().x, SpriteTexture->getSize().y) * Scale);
		};
	};

	if(CurrentAnimation != NULL && CurrentAnimation->Frames.size() && time(NULL) - LastFrameUpdate > FrameInterval)
	{
		LastFrameUpdate = time(NULL) - (time(NULL) - LastFrameUpdate - FrameInterval);

		CurrentAnimation->CurrentFrame++;

		if(CurrentAnimation->CurrentFrame >= CurrentAnimation->Frames.size())
			CurrentAnimation->CurrentFrame = CurrentAnimation->Repeating ? 0 : CurrentAnimation->Frames.size() - 1;

		Vector2 Frame = CurrentAnimation->Frames[CurrentAnimation->CurrentFrame];

		Options = Options.Crop(CropMode::CropTiled, Rect(FrameSize.x, Frame.x, FrameSize.y, Frame.y));
	};
};

void AnimatedSprite::StopAnimation()
{
	CurrentAnimation = NULL;
};
