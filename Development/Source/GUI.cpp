#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <json/json.h>
#include <sstream>
#include "SuperSmartPointer.hpp"
#include "StringID.hpp"
#include "GenericConfig.hpp"
#include "Delegate.h"
#include "Signal.h"
using namespace Gallant;
#include "Math.hpp"
#include "Input.hpp"
#include "Sprite.hpp"
#include "Future.hpp"
#include "StringUtils.hpp"
#include "GUI.hpp"

Rect MeasureTextSimple(const sf::String &Str, sf::Font *Font,
										unsigned long FontSize)
{
	if(!Font)
		return Rect();

	//Workaround for newline strings
	if(Str.getSize() == 0)
		return Rect(0, 0, 0, (float)FontSize);

	sf::Text Text;
	Text.setFont(*Font);
	Text.setCharacterSize(FontSize);
	Text.setString(Str);

	sf::FloatRect InRect = Text.getLocalBounds();

	return Rect(InRect.left, InRect.width, InRect.top, InRect.height);
};

void FitTextAroundLength(const sf::String &Str, sf::Font *Font,
						 unsigned long InFontSize, const float &LengthInPixels, unsigned long *OutFontSize)
{
	if(OutFontSize == NULL || LengthInPixels <= 0)
		return;

	*OutFontSize = InFontSize;

	Vector2 MeasuredText;

	while(MeasureTextSimple(Str, Font, *OutFontSize).ToFullSize().x > LengthInPixels)
	{
		(*OutFontSize)--;
	};
};

/*!
*	Formatting logic:
*	#1: Split Words and Lines
*	#2: For each word, keep adding it together until you exceed either size.x or size.y
*	#3: Check if we can recover by verifying if the old sentence works.
*	If it does, add a new line and continue if the size.y is less than the text height.
*/
std::vector<sf::String> FitTextOnRect(const sf::String &String, sf::Font *Font,
													   long FontSize, const Vector2 &Size)
{
	std::vector<sf::String> Lines;

	std::wstring str = String.toWideString();

	std::vector<std::wstring> Fragments;

	//Extract Fragments

	unsigned long Offset = 0, PreviousOffset = 0;

	for(;;)
	{
		PreviousOffset = Offset;

		long MatchSpace = str.find(L' ', PreviousOffset), MatchNewLine = str.find(L'\n', PreviousOffset);
		long Match = std::wstring::npos;

		if(MatchSpace != -1)
		{
			Match = MatchSpace;
		};

		if(MatchNewLine != -1 && (MatchSpace == -1 || MatchNewLine < MatchSpace))
		{
			Match = MatchNewLine;
		};

		if(Match == std::wstring::npos)
		{
			if(Offset < str.length())
			{
				Fragments.push_back(str.substr(Offset));
			};

			break;
		};

		if(Match - PreviousOffset > 0)
		{
			Fragments.push_back(str.substr(PreviousOffset, Match - PreviousOffset));

			if(Match == MatchNewLine)
				Fragments.push_back(L"\n");
		}
		else if(Match == MatchNewLine) //Add empty newlines
		{
			Fragments.push_back(L"\n");
		};

		Offset = Match + 1;
	};

	Vector2 CurrentSize;

	std::wstringstream stream, stream2;

	bool First = true;

	std::wstring CurrentText;
	long NewLineIndex = -1;

	while(CurrentSize.y < Size.y)
	{
		if(!First)
			stream << " ";

		if(First)
			First = false;

		if(Fragments.size() && Fragments[0].at(0) != L'\n')
			stream << Fragments[0];

		if(Fragments.size() && ((NewLineIndex = Fragments[0].find(L'\n')) == 0 || NewLineIndex == 1))
		{
			if(!First)
				stream.str(stream.str().substr(0, stream.str().length() - 1));

			Lines.push_back(stream.str());

			CurrentText += stream.str() + L"\n";

			stream.str(L"");
			stream2.str(L"");

			Fragments.erase(Fragments.begin());

			First = true;

			continue;
		};

		CurrentSize = MeasureTextSimple(CurrentText + stream.str(), Font, FontSize).ToFullSize();

		//Early-out
		if(CurrentSize.y > Size.y)
		{
			return Lines;
		};

		if(CurrentSize.x > Size.x)
		{
			//Recover old sentence
			if(stream2.str().length() != 0)
				stream.str(stream2.str());

			CurrentSize = MeasureTextSimple(stream.str(), Font, FontSize).ToFullSize();

			if(CurrentSize.x > Size.x) //So by default it would exceed size
			{
				return Lines;
			};

			//Verify the old text
			CurrentSize = MeasureTextSimple(CurrentText + stream.str(), Font, FontSize).ToFullSize();

			bool IgnoreNewLine = CurrentText.length() && CurrentText[CurrentText.length() - 1] == '\n';

			if(!IgnoreNewLine)
			{
				//Remove extra space
				if(!First)
				{
					CurrentText = CurrentText.substr(0, CurrentText.length() - 1);
				};

				CurrentText += L"\n";
			};

			//If we added text, add that line to our Lines and reset everything
			if(stream.str().length())
			{
				Lines.push_back(stream.str());

				stream.str(L"");
				stream2.str(L"");
			};

			stream.str(L"");
			stream2.str(L"");

			First = true;

			continue;
		};

		//Save the last working text here
		if(Fragments.size())
		{
			stream2.str(stream.str());

			Fragments.erase(Fragments.begin());
		}
		else //Final check here
		{
			CurrentSize = MeasureTextSimple(CurrentText + stream.str(), Font, FontSize).ToFullSize();

			if(CurrentSize.x > Size.x || CurrentSize.y > Size.y)
			{
				return Lines;
			};

			Lines.push_back(stream.str());

			return Lines;
		};
	};

	if(stream.str().length())
		Lines.push_back(stream.str());

	return Lines;
};

void RenderTextSimple(sf::RenderWindow &GameWindow, const sf::String &String, sf::Font *Font, unsigned long FontSize,
									   const Vector4 &Color, const Vector2 &Position)
{
	if(!Font)
		return;

	Vector2 ActualPosition = Position;

	GameWindow.pushGLStates();

	const_cast<sf::Texture &>(Font->getTexture(FontSize)).setSmooth(false);

	static sf::Text Text;

	if(Text.getFont() != Font)
		Text.setFont(*Font);

	if(Text.getCharacterSize() != FontSize)
		Text.setCharacterSize(FontSize);

	if(Text.getString() != String)
		Text.setString(String);

	sf::Color ActualTextColor((unsigned char)(Color.x * 255),
		(unsigned char)(Color.y * 255),
		(unsigned char)(Color.z * 255),
		(unsigned char)(Color.w * 255));

	Text.setPosition(ActualPosition.x, ActualPosition.y);
	Text.setColor(ActualTextColor);

	GameWindow.draw(Text);

	GameWindow.popGLStates();

	//SFML 2 BUG
	glMatrixMode(GL_MODELVIEW);
};

SuperSmartPointer<sf::Texture> LoadTexture(const std::string &FileName)
{
	SuperSmartPointer<sf::Texture> Out(new sf::Texture());

	if(!Out->loadFromFile(FileName))
		Out.Dispose();

	return Out;
};

UILayout::~UILayout()
{
	while(Elements.begin() != Elements.end())
	{
		if(Elements.begin()->second.Get())
		{
			Owner->RemoveElement(Elements.begin()->first);

			Elements.begin()->second.Dispose();
		};

		Elements.erase(Elements.begin());
	};
};

SuperSmartPointer<UIPanel> UILayout::FindPanelById(StringID ID)
{
	ElementMap::iterator it = Elements.find(ID);

	if(it == Elements.end())
		return SuperSmartPointer<UIPanel>();

	return it->second;
};

SuperSmartPointer<UIPanel> UILayout::FindPanelByName(const std::string &Name)
{
	ElementMap::iterator it = Elements.find(MakeStringID(Name));

	if(it == Elements.end())
		return SuperSmartPointer<UIPanel>();

	return it->second;
};

#define CHECKJSONVALUE(Value, type)\
	if(!Value.isNull())\
{\
	printf("While parsing a layout: Value '%s' is non-null and not the expected type '%s'", Value.toStyledString().c_str(), #type);\
};

void UIManager::CopyElementsToLayout(SuperSmartPointer<UILayout> TheLayout, Json::Value &Elements, UIPanel *Parent, const std::string &ParentElementName)
{
	if(Elements.type() != Json::ValueType::arrayValue)
		return;

	for(unsigned long i = 0; i < Elements.size(); i+=2)
	{
		Json::Value Value = Elements[i];

		if(!Value.isString())
		{
			CHECKJSONVALUE(Value, string);

			continue;
		};

		std::string ElementName = Value.asString();
		std::string ElementIDName = TheLayout->Name + "." + (ParentElementName.length() ? ParentElementName + "." : "") + ElementName;
		StringID ElementID = MakeStringID(ElementIDName);

		Json::Value &Data = Elements[i + 1];

		Value = Data.get("Control", Json::Value());

		if(!Value.isString())
		{
			CHECKJSONVALUE(Value, string);

			continue;
		};

		std::string Control = Value.asString();

		if(Control.length() == 0)
		{
			printf("Unable to add widget '%s' on layout '%s' due to missing 'Control' property", ElementName.c_str(), TheLayout->Name.c_str());

			continue;
		};

		SuperSmartPointer<UIPanel> Panel;

		Control = StringUtils::ToUpperCase(Control);

		if(Control == "WINDOW")
		{
			Panel.Reset(new UIWindow(this));
		}
		else if(Control == "FRAME")
		{
			Panel.Reset(new UIFrame(this));
		}
		else if(Control == "BUTTON")
		{
			Panel.Reset(new UIButton(this));
		}
		else if(Control == "CHECKBOX")
		{
			Panel.Reset(new UICheckBox(this));
		}
		else if(Control == "SPRITE")
		{
			Panel.Reset(new UISprite(this));
		}
		else if(Control == "TEXT")
		{
			Panel.Reset(new UIText(this));
		}
		else if(Control == "GROUP")
		{
			Panel.Reset(new UIGroup(this));
		}
		else if(Control == "TEXTBOX")
		{
			Panel.Reset(new UITextBox(this, false));
		}
		else if(Control == "LIST")
		{
			Panel.Reset(new UIList(this));
		}
		else if(Control == "SCROLLABLEFRAME")
		{
			Panel.Reset(new UIScrollableFrame(this));
		}
		else if(Control == "HSCROLL")
		{
			Panel.Reset(new UIScrollbar(this, false));
		}
		else if(Control == "VSCROLL")
		{
			Panel.Reset(new UIScrollbar(this, true));
		};

		if(Panel.Get() == NULL)
		{
			printf("Failed to read a widget '%s' from a layout '%s': Invalid control '%s'", ElementName.c_str(),
				TheLayout->Name.c_str(), Control.c_str());

			return;
		};

		Json::Value EnabledValue = Data.get("Enabled", Json::Value(true)),
			KeyboardInputEnabledValue = Data.get("KeyboardInput", Json::Value(true)),
			MouseInputEnabledValue = Data.get("MouseInput", Json::Value(true)),
			AlphaValue = Data.get("Opacity", Json::Value(1.0)),
			VisibleValue = Data.get("Visible", Json::Value(true));

		if(EnabledValue.isBool())
		{
			Panel->SetEnabled(EnabledValue.asBool());
		}
		else
		{
			CHECKJSONVALUE(EnabledValue, bool);
		};

		if(KeyboardInputEnabledValue.isBool())
		{
			Panel->SetKeyboardInputEnabled(KeyboardInputEnabledValue.asBool());
		}
		else
		{
			CHECKJSONVALUE(KeyboardInputEnabledValue, bool);
		};

		if(MouseInputEnabledValue.isBool())
		{
			Panel->SetMouseInputEnabled(MouseInputEnabledValue.asBool());
		}
		else
		{
			CHECKJSONVALUE(MouseInputEnabledValue, bool);
		};

		if(AlphaValue.isDouble())
		{
			Panel->SetAlpha((float)AlphaValue.asDouble());
		}
		else
		{
			CHECKJSONVALUE(AlphaValue, double);
		};

		if(VisibleValue.isBool())
		{
			Panel->SetVisible(VisibleValue.asBool());
		}
		else
		{
			CHECKJSONVALUE(VisibleValue, bool);
		};

		float x, y, w, h;

		Vector2 ParentSize = Parent ? Parent->GetSize() : Vector2(this->GetOwner()->getSize().x, this->GetOwner()->getSize().y);

		Value = Data.get("Wide", Json::Value("0"));
		std::string Temp;

		if(Value.isString())
		{
			Temp = Value.asString();

			if(Temp.rfind('%') != std::string::npos)
			{
				sscanf(Temp.c_str(), "%f%%", &w);
				w = ParentSize.x * w / 100.f;
			}
			else
			{
				sscanf(Temp.c_str(), "%f", &w);
			};
		}
		else
		{
			CHECKJSONVALUE(Value, string);
		};

		Value = Data.get("Tall", Json::Value("0"));

		if(Value.isString())
		{
			Temp = Value.asString();

			if(Temp.rfind('%') != std::string::npos)
			{
				sscanf(Temp.c_str(), "%f%%", &h);
				h = ParentSize.y * h / 100.f;
			}
			else
			{
				sscanf(Temp.c_str(), "%f", &h);
			};
		}
		else
		{
			CHECKJSONVALUE(Value, string);
		};

		Panel->SetSize(Vector2(w, h));

		TheLayout->Elements[MakeStringID(ElementName)] = Panel;

		if(!AddElement(ElementID, Panel))
			return;

		if(Parent)
			Parent->AddChildren(Panel);

		if(Control == "BUTTON")
		{
			UIButton *Button = Panel.AsDerived<UIButton>();
			Value = Data.get("Caption", Json::Value());

			if(Value.isString())
			{
				Button->Caption = Value.asString();
			}
			else
			{
				CHECKJSONVALUE(Value, string);
			};

			Value = Data.get("FontSize", Json::Value((Json::Value::Int)Button->FontSize));

			if(Value.isInt())
			{
				long FontSize = Value.asInt();

				Button->FontSize = FontSize;
			}
			else
			{
				CHECKJSONVALUE(Value, int);
			};
		}
		else if(Control == "CHECKBOX")
		{
			Value = Data.get("Checked", Json::Value(false));

			if(Value.isBool())
			{
				bool Checked = Value.asBool();

				Panel.AsDerived<UICheckBox>()->SetChecked(Checked);
			}
			else
			{
				CHECKJSONVALUE(Value, bool);
			};
		}
		else if(Control == "SPRITE")
		{
			UISprite *TheSprite = Panel.AsDerived<UISprite>();
			Value = Data.get("Path", Json::Value(""));

			if(Value.isString())
			{
				std::string FileName = Value.asString();

				if(FileName.length() == 0)
				{
					printf("Detected UISprite '%s' on Layout '%s' without a path!", ElementName.c_str(), TheLayout->Name.c_str());
				}
				else
				{
					SuperSmartPointer<sf::Texture> SpriteTexture = LoadTexture(FileName);

					if(!SpriteTexture.Get())
					{
						printf("Unable to load texture '%s' for UI Sprite '%s' on Layout '%s'", FileName.c_str(), ElementName.c_str(),
							TheLayout->Name.c_str());
					}
					else
					{
						TheSprite->TheSprite.SpriteTexture = SpriteTexture;
						TheSprite->TheSprite.Options.Scale(Panel->GetSize() != Vector2() ? Panel->GetSize() /
							Vector2(SpriteTexture->getSize().x, SpriteTexture->getSize().y) : Vector2(1, 1));
					};
				};
			}
			else
			{
				CHECKJSONVALUE(Value, string);
			};

			Value = Data.get("CropTiled", Json::Value(""));

			if(Value.isString())
			{
				std::string CropTiledString = Value.asString();

				if(CropTiledString.length() && TheSprite->TheSprite.SpriteTexture.Get())
				{
					Vector2 FrameSize, FrameID;

					sscanf(CropTiledString.c_str(), "%f, %f, %f, %f", &FrameSize.x, &FrameSize.y, &FrameID.x, &FrameID.y);

					TheSprite->TheSprite.Options.Scale(FrameSize / Vector2(TheSprite->TheSprite.SpriteTexture->getSize().x,
						TheSprite->TheSprite.SpriteTexture->getSize().y)).Crop(CropMode::CropTiled,
						Rect(FrameSize.x, FrameID.x, FrameSize.y, FrameID.y));
				};
			}
			else
			{
				CHECKJSONVALUE(Value, string);
			};
		}
		else if(Control == "WINDOW")
		{
			Value = Data.get("Closed", Json::Value(false));

			if(Value.isBool())
			{
				bool Closed = Value.asBool();

				Panel.AsDerived<UIWindow>()->SetClosed(Closed);
			}
			else
			{
				CHECKJSONVALUE(Value, bool);
			};
		}
		else if(Control == "TEXT")
		{
			Value = Data.get("ExpandHeight", Json::Value(false));
			bool AutoExpand = Value.isBool() && Value.asBool();
			Value = Data.get("FontSize", Json::Value((Json::Value::Int)GetDefaultFontSize()));
			long FontSize = GetDefaultFontSize();

			if(Value.isInt())
			{
				FontSize = Value.asInt();
			}
			else
			{
				CHECKJSONVALUE(Value, int);
			};

			std::string Text;
			Value = Data.get("Text", Json::Value(""));

			if(Value.isString())
			{
				Text = Value.asString();
			}
			else
			{
				CHECKJSONVALUE(Value, string);
			};

			unsigned long Alignment = 0;

			Value = Data.get("Alignment", Json::Value(""));

			if(Value.isString())
			{
				std::string AlignmentString = StringUtils::ToUpperCase(Value.asString());

				std::vector<std::string> Fragments = StringUtils::Split(AlignmentString, '|');

				for(unsigned long j = 0; j < Fragments.size(); j++)
				{
					if(Fragments[j] == "CENTER")
					{
						Alignment |= UITextAlignment::Center;
					}
					else if(Fragments[j] == "LEFT")
					{
						Alignment |= UITextAlignment::Left;
					}
					else if(Fragments[j] == "RIGHT")
					{
						Alignment |= UITextAlignment::Right;
					}
					else if(Fragments[j] == "VCENTER")
					{
						Alignment |= UITextAlignment::VCenter;
					};
				};

				if(Fragments.size() == 0)
					Alignment = UITextAlignment::Left;
			}
			else
			{
				CHECKJSONVALUE(Value, string);
			};

			UIText *TheText = Panel.AsDerived<UIText>();

			TheText->TextAlignment = Alignment;
			TheText->FontSize = FontSize;
			TheText->SetText(Text, AutoExpand);
		}
		else if(Control == "TEXTBOX")
		{
			UITextBox *TheTextBox = Panel.AsDerived<UITextBox>();

			Value = Data.get("Text", Json::Value(""));
			std::string Text;

			if(Value.isString())
			{
				Text = Value.asString();
			}
			else
			{
				CHECKJSONVALUE(Value, string);
			};

			long FontSize = TheTextBox->FontSize;

			Value = Data.get("FontSize", Json::Value((Json::Value::Int)TheTextBox->FontSize));

			if(Value.isInt())
			{
				FontSize = Value.asInt();;
			}
			else
			{
				CHECKJSONVALUE(Value, int);
			};

			TheTextBox->FontSize = FontSize;
			TheTextBox->SetText(Text);
		}
		else if(Control == "LIST")
		{
			UIList *TheList = Panel.AsDerived<UIList>();

			Value = Data.get("Elements", Json::Value(""));

			if(Value.isString())
			{
				std::string ElementString = Value.asString();

				std::vector<std::string> Items = StringUtils::Split(ElementString, '|');

				TheList->Items = Items;
			}
			else
			{
				CHECKJSONVALUE(Value, string);
			};

			long FontSize = TheList->GetManager()->GetDefaultFontSize();

			Value = Data.get("FontSize", Json::Value((Json::Value::Int)TheList->GetManager()->GetDefaultFontSize()));

			if(Value.isInt())
			{
				FontSize = Value.asInt();
			}
			else
			{
				CHECKJSONVALUE(Value, int);
			};

			TheList->FontSize = FontSize;
		};

		Panel->PerformLayout();

		Value = Data.get("Xpos", Json::Value("0"));

		if(Value.isString())
		{
			Temp = Value.asString();

			if(StringUtils::ToUpperCase(Temp) == "CENTER")
			{
				x = (ParentSize.x - Panel->GetSize().x) / 2;
			}
			else if(Temp.rfind('%') != std::string::npos)
			{
				sscanf(Temp.c_str(), "%f%%", &x);
				x = ParentSize.x * x / 100.f;
			}
			else
			{
				sscanf(Temp.c_str(), "%f", &x);
			};
		}
		else
		{
			CHECKJSONVALUE(Value, string);
		};

		Value = Data.get("Ypos", Json::Value("0"));

		if(Value.isString())
		{
			Temp = Value.asString();

			if(StringUtils::ToUpperCase(Temp) == "CENTER")
			{
				y = (ParentSize.y - Panel->GetSize().y) / 2;
			}
			else if(Temp.rfind('%') != std::string::npos)
			{
				sscanf(Temp.c_str(), "%f%%", &y);
				y = ParentSize.y * y / 100.f;
			}
			else
			{
				sscanf(Temp.c_str(), "%f", &y);
			};
		}
		else
		{
			CHECKJSONVALUE(Value, string);
		};

		Value = Data.get("Tooltip", Json::Value(""));

		if(Value.isString() && Value.asString().length())
		{
			Panel->SetRespondsToTooltips(true);
			Panel->SetTooltipText(Value.asString());
		};

		Panel->SetPosition(Vector2(x, y));

		Json::Value Children = Data.get("Children", Json::Value());

		if(!Children.isArray())
			continue;

		CopyElementsToLayout(TheLayout, Children, Panel, (ParentElementName.length() ? (ParentElementName + ".") : "") + ElementName);
	};
};

bool UIManager::LoadLayouts(const std::string &Data, SuperSmartPointer<UIPanel> Parent)
{
	Json::Value Root;
	Json::Reader Reader;

	if(!Reader.parse(Data, Root))
	{
		printf("Failed to parse a GUI layout resource: %s", Reader.getFormatedErrorMessages().c_str());

		return false;
	};

	for(unsigned long i = 0; i < Root.size(); i+=2)
	{
		std::string LayoutName = Root[i].asString();
		Json::Value Elements = Root[i + 1];
		SuperSmartPointer<UILayout> Layout(new UILayout());
		Layout->Name = LayoutName;
		Layout->Owner = this;

		CopyElementsToLayout(Layout, Elements, Parent, "");

		StringID LayoutID = MakeStringID((Parent.Get() ? Parent->GetLayout()->Name + "_" : "") + LayoutName);

		LayoutMap::iterator it = Layouts.find(LayoutID);

		if(it != Layouts.end())
		{
			printf("Found duplicate layout '%s', erasing old.", LayoutName.c_str());

			Layouts.erase(it);
		};

		Layouts[LayoutID] = Layout;
	};

	return true;
};

void UIManager::ClearLayouts()
{
	while(Layouts.begin() != Layouts.end())
	{
		Layouts.begin()->second.Dispose();
		Layouts.erase(Layouts.begin());
	};
};

void RemoveElementFuture(std::vector<unsigned char> &Stream)
{
	StringID ID = 0;
	UIManager *Manager = NULL;

	memcpy(&Stream[0], &ID, sizeof(StringID));
	memcpy(&Stream[sizeof(StringID)], &Manager, sizeof(UIManager *));

	Manager->RemoveElement(ID);
};

void UIPanel::AddChildren(UIPanel *Child)
{
	if(Child->Manager != Manager)
		return;

	if(Child->ParentValue)
	{
		Child->ParentValue->RemoveChild(Child);
	};

	Children.push_back(Child);
	Child->ParentValue = GetManager()->GetElement(ID);
};

void UIPanel::Clear()
{
	while(Children.size())
	{
		unsigned long OldSize = Children.size();

		Manager->RemoveElement(Children[0]->GetID());

		unsigned long NewSize = Children.size();

		if(NewSize == OldSize)
			Children.erase(Children.begin());
	};
};

UIPanel::~UIPanel()
{
	if(ParentValue)
		ParentValue->RemoveChild(this);

	Clear();
};

void UIFrame::OnSkinChange()
{
	std::string TexturePath = Skin->GetString("Frame", "BackgroundTexture");

	BackgroundTexture = LoadTexture(TexturePath);

	std::string NinePatchRectValue = Skin->GetString("Frame", "TextureRect");

	sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
		&TextureRect.Top, &TextureRect.Bottom);

	SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
};

void UIFrame::Update(const Vector2 &ParentPosition)
{
	for(unsigned long i = 0; i < Children.size(); i++)
	{
		Children[i]->Update(ParentPosition + PositionValue);
	};
};

void UIFrame::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x ||
		ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->getSize().y))
		return;

	Sprite TheSprite;
	TheSprite.SpriteTexture = BackgroundTexture;
	TheSprite.Options.Position(ActualPosition).NinePatch(true, TextureRect).Scale(SizeValue);

	TheSprite.Draw(Renderer);

	for(unsigned long i = 0; i < Children.size(); i++)
	{
		if(Children[i]->IsVisible())
			Children[i]->Draw(ActualPosition, Renderer);
	};
};

void UIButton::OnSkinChange()
{
	std::string TexturePath = Skin->GetString("Button", "BackgroundTexture");

	NormalTexture = LoadTexture(TexturePath);

	TexturePath = Skin->GetString("Button", "FocusedTexture");

	FocusedTexture = LoadTexture(TexturePath);

	std::string NinePatchRectValue = Skin->GetString("Button", "TextureRect");

	sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
		&TextureRect.Top, &TextureRect.Bottom);

	std::string FontSizeValue = Skin->GetString("Button", "FontSize");

	sscanf(FontSizeValue.c_str(), "%u", &FontSize);

	std::string FontColorValue = Skin->GetString("Button", "FontColor");

	sscanf(FontColorValue.c_str(), "%f,%f,%f,%f", &FontColor.x, &FontColor.y, &FontColor.z, &FontColor.w);

	std::string LabelOffsetValue = Skin->GetString("Button", "LabelOffset");

	sscanf(LabelOffsetValue.c_str(), "%f,%f", &LabelOffset.x, &LabelOffset.y);

	SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
};

void UIButton::PerformLayout()
{
	Rect Size = MeasureTextSimple(Caption, Manager->GetDefaultFont(), FontSize);

	Vector2 ActualSize = Size.Position() + Size.Size();

	if(ActualSize.x > SizeValue.x)
	{
		SizeValue.x = ActualSize.x + 10;
	};

	if(ActualSize.y > SizeValue.y)
	{
		SizeValue.y = ActualSize.y + 10;
	};
};

void UIButton::Update(const Vector2 &ParentPosition)
{
	PerformLayout();
};

void UIButton::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x || ActualPosition.y + SizeValue.y < 0 ||
		ActualPosition.y > Renderer->getSize().y))
		return;

	Rect Size = MeasureTextSimple(Caption, Manager->GetDefaultFont(), FontSize);

	Vector2 ActualSize = Size.ToFullSize();

	Sprite TheSprite;
	TheSprite.SpriteTexture = ClickPressed ? FocusedTexture : NormalTexture;
	TheSprite.Options.Position(ActualPosition).NinePatch(true, TextureRect).Scale(SizeValue);

	TheSprite.Draw(Renderer);

	//Not /2'ing the Y axis because it works better this way for some reason
	Vector2 Offset = Vector2((SizeValue.x - ActualSize.x) / 2 + LabelOffset.x, LabelOffset.y);

	RenderTextSimple(*Renderer, Caption, Manager->GetDefaultFont(), FontSize,
		FontColor, ActualPosition + Offset);

	glColor4f(1, 1, 1, 1);
};

UITextBox::UITextBox(UIManager *Manager, bool _MultiLine) : UIPanel(Manager), FontSize(12), CursorPosition(0),
	TextOffset(0), MultiLine(_MultiLine)
{
	OnMouseJustPressed.Connect(this, &UITextBox::OnMouseJustPressedTextBox);
	OnKeyJustPressed.Connect(this, &UITextBox::OnKeyJustPressedTextBox);
	OnCharacterEntered.Connect(this, &UITextBox::OnCharacterEnteredTextBox);
};

void UITextBox::OnMouseJustPressedTextBox(UIPanel *This, const InputCenter::MouseButtonInfo &o)
{
	if(this == Manager->GetFocusedElement() && Text.getSize() > 0 && o.Name == sf::Mouse::Left)
	{
		//Since we're already pressed, we must be inside it, so we just need to care about the X axis

		Vector2 ActualPosition = PositionValue + Vector2((float)Padding, 0) + GetParentPosition();

		std::wstring WideString = Text.toWideString();

		unsigned long X = 0;

		Rect TextSize = MeasureTextSimple(WideString.substr(TextOffset), Manager->GetDefaultFont(), FontSize);

		for(unsigned long i = 0; i < WideString.length() - TextOffset; i++)
		{
			Vector2 Size = MeasureTextSimple(WideString.substr(TextOffset, i + 1),
				Manager->GetDefaultFont(), FontSize).ToFullSize();

			if(ActualPosition.x + Size.x + TextSize.Left > GetManager()->GetInput()->MousePosition.x)
				break;

			X++;
		};

		CursorPosition = X;
	};
};

void UITextBox::OnKeyJustPressedTextBox(UIPanel *This, const InputCenter::KeyInfo &o)
{
	if(this == Manager->GetFocusedElement())
	{
		if(o.Name == sf::Keyboard::Left)
		{
			if(CursorPosition == 0 && TextOffset > 0)
			{
				TextOffset--;
			}
			else if(CursorPosition == 0 && TextOffset == 0)
			{
				return;
			}
			else
			{
				CursorPosition--;
			};
		}
		else if(o.Name == sf::Keyboard::Right)
		{
			if(CursorPosition + TextOffset + 1 > Text.getSize())
				return;

			std::wstring ActualText = Text.toWideString();

			std::wstring str = ActualText.substr(TextOffset, CursorPosition + 1);

			Vector2 TextSize = MeasureTextSimple(str, Manager->GetDefaultFont(), FontSize).ToFullSize();

			if(TextSize.x >= SizeValue.x)
			{
				TextOffset++;
			}
			else
			{
				CursorPosition++;
			};
		};
	};
};

void UITextBox::OnCharacterEnteredTextBox(UIPanel *This)
{
	if(this == Manager->GetFocusedElement())
	{
		std::wstring ActualText = Text.toWideString();

		if(GetManager()->GetInput()->Character == 8) //Hardcoded Backspace
		{
			if(ActualText.length() == 0)
				return;

			ActualText.erase(TextOffset + CursorPosition - 1);

			if(TextOffset > 0)
			{
				TextOffset--;
			}
			else if(CursorPosition > 0)
			{
				CursorPosition--;
			};
		}
		else
		{
			ActualText = ActualText.substr(0, TextOffset + CursorPosition) + GetManager()->GetInput()->Character +
				ActualText.substr(TextOffset + CursorPosition);

			CursorPosition++;

			Vector2 TextSize = MeasureTextSimple(ActualText.substr(TextOffset),
				Manager->GetDefaultFont(), FontSize).ToFullSize();

			while(TextSize.x >= SizeValue.x)
			{
				CursorPosition--;
				TextOffset++;

				TextSize = MeasureTextSimple(ActualText.substr(TextOffset),
					Manager->GetDefaultFont(), FontSize).ToFullSize();
			};
		};

		Text = ActualText;
	};
};

void UITextBox::OnSkinChange()
{
	std::string TexturePath = Skin->GetString("TextBox", "BackgroundTexture");

	BackgroundTexture = LoadTexture(TexturePath);

	std::string NinePatchRectValue = Skin->GetString("TextBox", "TextureRect");

	sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
		&TextureRect.Top, &TextureRect.Bottom);

	FontSize = Manager->GetDefaultFontSize();

	std::string PaddingValue = Skin->GetString("TextBox", "Padding");

	sscanf(PaddingValue.c_str(), "%u", &Padding);

	SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
};

void UITextBox::PerformLayout()
{
	if(SizeValue.y < FontSize + 10)
	{
		SizeValue.y = (float)FontSize + 10;
	};
};

void UITextBox::Update(const Vector2 &ParentPosition)
{
	PerformLayout();
};

#define TEXTBOX_BORDER_SIZE 1

void UITextBox::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;
	Vector2 ActualSize = SizeValue + Vector2(Padding * 2.0f, 0);

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x ||
		ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->getSize().y))
		return;


	Sprite TheSprite;
	TheSprite.SpriteTexture = BackgroundTexture;
	TheSprite.Options.Position(ActualPosition).NinePatch(true, TextureRect).Scale(ActualSize);

	TheSprite.Draw(Renderer);

	std::wstring WideString = Text.toWideString();

	Rect TextSize = MeasureTextSimple(WideString.substr(TextOffset), Manager->GetDefaultFont(), FontSize);

	Vector2 ActualTextSize = TextSize.Position() + TextSize.Size();

	//Not centering, and using SizeValue otherwise text pops
	Vector2 Offset = Vector2(-TextSize.Left + Padding, (SizeValue.y * 0.25f));

	unsigned long Count = 0;

	{
		for(unsigned long i = 0; i < WideString.size() - TextOffset; i++, Count++)
		{
			Vector2 Size = MeasureTextSimple(WideString.substr(TextOffset, i + 1),
				Manager->GetDefaultFont(), FontSize).ToFullSize();

			if(Size.x >= SizeValue.x)
				break;
		};
	};

	RenderTextSimple(*Renderer, WideString.substr(TextOffset, Count),
		Manager->GetDefaultFont(), FontSize, Vector4(0, 0, 0, 1), ActualPosition + Offset);

	if(this == Manager->GetFocusedElement())
	{
		//Draw cursor
		float X = 0;

		for(unsigned long i = 0; i < CursorPosition; i++)
		{
			Vector2 Size = MeasureTextSimple(WideString.substr(TextOffset, i + 1), Manager->GetDefaultFont(),
				FontSize).ToFullSize();

			if(Size.x >= SizeValue.x + Offset.x)
				break;

			X = Size.x;
		};

		glBindTexture(GL_TEXTURE_2D, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glColor4f(0, 0, 0, AlphaValue);

		Vector2 BasePosition = ActualPosition + Vector2(X + Padding, 0);

		Vector2 LineVertices[2] = {
			BasePosition + Vector2(0, SizeValue.y * 0.05f),
			BasePosition + Vector2(0, SizeValue.y * 0.95f)
		};

		glVertexPointer(2, GL_FLOAT, 0, LineVertices);

		glLineWidth(TEXTBOX_BORDER_SIZE);

		glDrawArrays(GL_LINES, 0, 2);

		glLineWidth(1);
	};

	glDisableClientState(GL_VERTEX_ARRAY);

	glColor4f(1, 1, 1, 1);
};

void UIScrollableFrame::OnSkinChange()
{
	VerticalScroll->SetSkin(Skin);
	HorizontalScroll->SetSkin(Skin);
};

void UIScrollableFrame::MakeScrolls()
{
	VerticalScroll.Dispose();
	HorizontalScroll.Dispose();
	VerticalScroll.Reset(new UIScrollbar(GetManager(), true));
	HorizontalScroll.Reset(new UIScrollbar(GetManager(), false));

	GetManager()->AddElement(MakeStringID(StringUtils::MakeIntString((unsigned long)this, true) + "_VERTICALSCROLL"), VerticalScroll);

	GetManager()->AddElement(MakeStringID(StringUtils::MakeIntString((unsigned long)this, true) + "_HORIZONTALSCROLL"), HorizontalScroll);

	AddChildren(VerticalScroll);
	AddChildren(HorizontalScroll);
};

void UIScrollableFrame::Update(const Vector2 &ParentPosition)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	Vector2 ChildrenSize = GetChildrenSize();

	if(ChildrenSize.y > SizeValue.y)
	{
		VerticalScroll->SetSize(Vector2(SCROLLBAR_DRAGGABLE_SIZE, SizeValue.y) - VerticalScroll->GetExtraSize());
		VerticalScroll->SetPosition(Vector2(SizeValue.x - VerticalScroll->GetSize().x, 0) +
			Vector2(-VerticalScroll->GetExtraSize().x / 2.f, VerticalScroll->GetExtraSize().y / 2.f));
		TranslationValue.y = (VerticalScroll->Value() / (float)VerticalScroll->MaxValue) * (ChildrenSize.y - SizeValue.y);
	};

	VerticalScroll->SetVisible(ChildrenSize.y > SizeValue.y);

	if(ChildrenSize.x > SizeValue.x)
	{
		HorizontalScroll->SetSize(Vector2(SizeValue.x, SCROLLBAR_DRAGGABLE_SIZE) - HorizontalScroll->GetExtraSize());
		HorizontalScroll->SetPosition(Vector2(0, SizeValue.y - HorizontalScroll->GetSize().y) +
			Vector2(HorizontalScroll->GetExtraSize().x / 2.f, -HorizontalScroll->GetExtraSize().y / 2.f));
		TranslationValue.x = (HorizontalScroll->Value() / (float)HorizontalScroll->MaxValue) * (ChildrenSize.x - SizeValue.x);
	};

	HorizontalScroll->SetVisible(ChildrenSize.x > SizeValue.x);

	if(VerticalScroll->IsVisible() && HorizontalScroll->IsVisible())
	{
		VerticalScroll->SetSize(VerticalScroll->GetSize() - Vector2(0, HorizontalScroll->GetExtraSize().y + HorizontalScroll->GetSize().y));
		HorizontalScroll->SetSize(HorizontalScroll->GetSize() - Vector2(VerticalScroll->GetExtraSize().x + VerticalScroll->GetSize().x, 0));
	};

	for(unsigned long i = 0; i < Children.size(); i++)
	{
		Children[i]->Update(ActualPosition);
	};
};

void UIScrollableFrame::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x ||
		ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->getSize().y))
		return;

	Vector2 Vertices[6] = {
		ActualPosition + Vector2(-TEXTBOX_BORDER_SIZE, -TEXTBOX_BORDER_SIZE),
		ActualPosition + Vector2(-TEXTBOX_BORDER_SIZE, SizeValue.y + TEXTBOX_BORDER_SIZE),
		ActualPosition + SizeValue + Vector2(TEXTBOX_BORDER_SIZE, TEXTBOX_BORDER_SIZE),
		ActualPosition + SizeValue + Vector2(TEXTBOX_BORDER_SIZE, TEXTBOX_BORDER_SIZE),
		ActualPosition + Vector2(SizeValue.x + TEXTBOX_BORDER_SIZE, -TEXTBOX_BORDER_SIZE),
		ActualPosition + Vector2(-TEXTBOX_BORDER_SIZE, -TEXTBOX_BORDER_SIZE),
	};

	glColor4f(0, 0, 0, AlphaValue);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, Vertices);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	Vertices[0] -= Vector2(-TEXTBOX_BORDER_SIZE, -TEXTBOX_BORDER_SIZE);
	Vertices[1] -= Vector2(-TEXTBOX_BORDER_SIZE, TEXTBOX_BORDER_SIZE);
	Vertices[2] -= Vector2(TEXTBOX_BORDER_SIZE, TEXTBOX_BORDER_SIZE);
	Vertices[3] -= Vector2(TEXTBOX_BORDER_SIZE, TEXTBOX_BORDER_SIZE);
	Vertices[4] -= Vector2(TEXTBOX_BORDER_SIZE, -TEXTBOX_BORDER_SIZE);
	Vertices[5] -= Vector2(-TEXTBOX_BORDER_SIZE, -TEXTBOX_BORDER_SIZE);

	glColor4f(0.3f, 0.3f, 0.3f, AlphaValue);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glColor4f(1, 1, 1, 1);

	Vector2 ChildrenSize = GetChildrenSize();

	glEnable(GL_SCISSOR_TEST);

	glScissor((GLsizei)ActualPosition.x, (GLsizei)(Renderer->getSize().y - ActualPosition.y - SizeValue.y),
		(GLsizei)(SizeValue.x - (ChildrenSize.y > SizeValue.y ? VerticalScroll->GetSize().x : 0)), (GLsizei)(SizeValue.y -
		(ChildrenSize.x > SizeValue.x ? HorizontalScroll->GetSize().y : 0)));

	for(unsigned long i = 2; i < Children.size(); i++)
	{
		Children[i]->Draw(ActualPosition - TranslationValue, Renderer);
	};

	glDisable(GL_SCISSOR_TEST);

	VerticalScroll->Draw(ActualPosition, Renderer);
	HorizontalScroll->Draw(ActualPosition, Renderer);
};

void UIList::OnSkinChange()
{
	FontSize = GetManager()->GetDefaultFontSize();

	std::string TexturePath = Skin->GetString("Menu", "SelectorBackgroundTexture");

	SelectorBackgroundTexture = LoadTexture(TexturePath);
};

void UIList::PerformLayout()
{
	float Height = 0;

	for(unsigned long i = 0; i < Items.size(); i++)
	{
		Vector2 Size = MeasureTextSimple(Items[i], Manager->GetDefaultFont(), FontSize).ToFullSize();

		float SizeY = (Size.y < FontSize * 1.15f ? FontSize * 1.15f : Size.y);

		Height += SizeY + 2;
	};

	if(SizeValue.y < Height)
		SizeValue.y = Height;
};

void UIList::OnItemClickCheck(UIPanel *Self)
{
	Vector2 ActualPosition = GetParentPosition() + PositionValue;
	AxisAlignedBoundingBox AABB;
	float Height = 0;

	AABB.min = ActualPosition;

	for(unsigned long i = 0; i < Items.size(); i++)
	{
		Vector2 Size = MeasureTextSimple(Items[i], Manager->GetDefaultFont(), FontSize).ToFullSize();

		float SizeY = (Size.y < FontSize * 1.15f ? FontSize * 1.15f : Size.y);

		AABB.min.y = ActualPosition.y + Height;
		AABB.max = ActualPosition + Vector2(ParentValue->GetSize().x - SCROLLBAR_DRAGGABLE_SIZE, Height + SizeY);

		Height += SizeY + 2;

		if(AABB.IsInside(GetManager()->GetInput()->MousePosition))
		{
			OnItemClick(this, i);

			return;
		};
	};
};

void UIList::Update(const Vector2 &ParentPosition)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;
	AxisAlignedBoundingBox AABB;
	float Height = 0;

	AABB.min = ActualPosition;

	for(unsigned long i = 0; i < Items.size(); i++)
	{
		Vector2 Size = MeasureTextSimple(Items[i], Manager->GetDefaultFont(), FontSize).ToFullSize();

		float SizeY = (Size.y < FontSize * 1.15f ? FontSize * 1.15f : Size.y);

		AABB.min.y = ActualPosition.y + Height;
		AABB.max = ActualPosition + Vector2(ParentValue->GetSize().x - SCROLLBAR_DRAGGABLE_SIZE, Height + SizeY);

		Height += SizeY + 2;

		if(AABB.IsInside(GetManager()->GetInput()->MousePosition))
		{
			OnItemMouseOver(this, i);

			return;
		};
	};
};

void UIList::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	if(!ParentValue)
		return;

	static AxisAlignedBoundingBox AABB;

	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x ||
		ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->getSize().y))
		return;

	float Height = 0;

	AABB.min = ActualPosition;

	for(unsigned long i = 0; i < Items.size(); i++)
	{
		Vector2 Size = MeasureTextSimple(Items[i], Manager->GetDefaultFont(), FontSize).ToFullSize();

		float SizeY = (Size.y < FontSize * 1.15f ? FontSize * 1.15f : Size.y);

		AABB.min.y = ActualPosition.y + Height;
		AABB.max = ActualPosition + Vector2(ParentValue->GetSize().x - SCROLLBAR_DRAGGABLE_SIZE, Height + SizeY);

		Height += SizeY + 2;

		if(AABB.IsInside(GetManager()->GetInput()->MousePosition))
		{
			Sprite TheSprite;
			TheSprite.SpriteTexture = SelectorBackgroundTexture;
			TheSprite.Options.Position(AABB.min.ToVector2()).Scale(Vector2(ParentValue->GetSize().x - SCROLLBAR_DRAGGABLE_SIZE, AABB.max.y - AABB.min.y) /
				Vector2(SelectorBackgroundTexture->getSize().x, SelectorBackgroundTexture->getSize().y));

			TheSprite.Draw(Renderer);
		};

		RenderTextSimple(*Renderer, Items[i], Manager->GetDefaultFont(), FontSize,
			GetManager()->GetDefaultFontColor(), Vector2(AABB.min.x, AABB.min.y));
	};

	glColor4f(1, 1, 1, 1);

	Vector3 Final = AABB.max - ActualPosition;

	SizeValue = Vector2(Final.x, Final.y);
};

void UIText::OnSkinChange()
{
	FontSize = Manager->GetDefaultFontSize();
	FontColor = Manager->GetDefaultFontColor();
};

void UIText::SetText(const sf::String &String, bool AutoExpandHeight)
{
	Text = String;
	Strings = FitTextOnRect(Text, Manager->GetDefaultFont(), FontSize,
		AutoExpandHeight ? Vector2(SizeValue.x, 9999999) : SizeValue);

	if(AutoExpandHeight)
	{
		SizeValue.y = SizeValue.y > Strings.size() * (FontSize + 4) ? SizeValue.y : Strings.size() * (FontSize + 4);
	};
};

const sf::String &UIText::GetText()
{
	return Text;
};

Vector2 UIText::GetTextSize()
{
	Vector2 Size;

	for(unsigned long i = 0; i < Strings.size(); i++)
	{
		Vector2 TextSize = MeasureTextSimple(Strings[i], Manager->GetDefaultFont(), FontSize).ToFullSize();
		Size.y += TextSize.y;

		if(Size.x < TextSize.x)
			Size.x = TextSize.x;
	};

	return Size;
};

void UIText::Update(const Vector2 &ParentPosition)
{
	//Do nothing
};

void UIText::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x ||
		ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->getSize().y))
		return;

	unsigned long YOffset = 0;

	if(TextAlignment & UITextAlignment::VCenter)
	{
		YOffset = (unsigned long)(SizeValue.y - (Strings.size() * (FontSize + 4))) / 2;
	};

	for(unsigned long i = 0, TextYOffset = YOffset; i < Strings.size(); i++, TextYOffset += FontSize + 4)
	{
		if(TextAlignment & UITextAlignment::Center)
		{
			RenderTextSimple(*Renderer, Strings[i], Manager->GetDefaultFont(), FontSize, FontColor,
				ActualPosition +
				Vector2((SizeValue.x - MeasureTextSimple(Strings[i], Manager->GetDefaultFont(),
				FontSize).ToFullSize().x) / 2, (float)TextYOffset));
		}
		else if(TextAlignment & UITextAlignment::Right)
		{
			RenderTextSimple(*Renderer, Strings[i], Manager->GetDefaultFont(), FontSize, FontColor,
				ActualPosition +
				Vector2(SizeValue.x - MeasureTextSimple(Strings[i], Manager->GetDefaultFont(),
				FontSize).ToFullSize().x, (float)TextYOffset));
		}
		else
		{
			RenderTextSimple(*Renderer, Strings[i], Manager->GetDefaultFont(), FontSize, FontColor,
				ActualPosition + Vector2(0, (float)TextYOffset));
		};
	};
};

void UISprite::PerformLayout()
{
	SizeValue = TheSprite.SpriteTexture ? Vector2(TheSprite.SpriteTexture->getSize().x, TheSprite.SpriteTexture->getSize().y) *
		TheSprite.Options.ScaleValue : Vector2();
};

void UISprite::Update(const Vector2 &ParentPosition)
{
	PerformLayout();

	Vector2 ActualPosition = ParentPosition + PositionValue;

	for(unsigned long i = 0; i < Children.size(); i++)
	{
		Children[i]->Update(ActualPosition);
	};
};

void UISprite::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x ||
		ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->getSize().y))
		return;

	TheSprite.Options = TheSprite.Options.Position(ActualPosition);
	TheSprite.Draw(Renderer);

	for(unsigned long i = 0; i < Children.size(); i++)
	{
		Children[i]->Draw(ActualPosition, Renderer);
	};
};

void UIScrollbar::OnSkinChange()
{
	std::string TexturePath = Skin->GetString("Scrollbar", "BackgroundTexture");

	BackgroundTexture = LoadTexture(TexturePath);

	std::string NinePatchRectValue = Skin->GetString("Scrollbar", "BackgroundTextureRect");

	sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &BackgroundTextureRect.Left, &BackgroundTextureRect.Right,
		&BackgroundTextureRect.Top, &BackgroundTextureRect.Bottom);

	TexturePath = Skin->GetString("Scrollbar", "HandleTexture");

	HandleTexture = LoadTexture(TexturePath);

	NinePatchRectValue = Skin->GetString("Scrollbar", "HandleTextureRect");

	sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &HandleTextureRect.Left, &HandleTextureRect.Right,
		&HandleTextureRect.Top, &HandleTextureRect.Bottom);

	std::string PaddingValue = Skin->GetString("Scrollbar", "Padding");

	sscanf(PaddingValue.c_str(), "%u", &Padding);

	std::string MinSizeValue = Skin->GetString("Scrollbar", "MinSize");

	sscanf(MinSizeValue.c_str(), "%u", &MinSize);

	SelectBoxExtraSize = Vector2(BackgroundTextureRect.Left + BackgroundTextureRect.Right, BackgroundTextureRect.Top + BackgroundTextureRect.Bottom);
};

void UIScrollbar::PerformLayout()
{
};

void UIScrollbar::Update(const Vector2 &ParentPosition)
{
	if(this == Manager->GetFocusedElement().Get() && GetManager()->GetInput()->MouseButtons[sf::Mouse::Left].Pressed)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue;

		unsigned long Steps = (MaxValue - MinValue) / ValueStep;

		AxisAlignedBoundingBox AABB;

		for(unsigned long i = 0; i <= Steps; i++)
		{
			float StepOffset = (Vertical ? (SizeValue.y + SelectBoxExtraSize.y) / Steps * i : (SizeValue.x + SelectBoxExtraSize.x) / Steps * i);

			AABB.min = ActualPosition + (Vertical ? Vector2((float)Padding, StepOffset + Padding - SelectBoxExtraSize.y / 2) : 
				Vector2(StepOffset + Padding - SelectBoxExtraSize.x / 2, (float)Padding)) - SelectBoxExtraSize / 2;
			AABB.max = ActualPosition + (Vertical ? Vector2(SizeValue.x - Padding * 2 + BackgroundTextureRect.Left + BackgroundTextureRect.Right,
				StepOffset + Padding - SelectBoxExtraSize.y / 2 + (float)MinSize) :
				Vector2(StepOffset + Padding - SelectBoxExtraSize.x / 2 + (float)MinSize,
				SizeValue.y - Padding * 2 + BackgroundTextureRect.Top + BackgroundTextureRect.Bottom)) + SelectBoxExtraSize / 2;

			if(AABB.IsInside(GetManager()->GetInput()->MousePosition))
			{
				bool NeedsChange = CurrentStep != i;

				CurrentStep = i;

				if(NeedsChange)
					OnChange(this);

				return;
			};
		};
	};
};

void UIScrollbar::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x ||
		ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->getSize().y))
		return;

	unsigned long Steps = (MaxValue - MinValue) / ValueStep;
	float StepOffset = Vertical ? (SizeValue.y + SelectBoxExtraSize.y) / Steps * CurrentStep : (SizeValue.x + SelectBoxExtraSize.x) / Steps * CurrentStep;

	Sprite TheSprite;
	TheSprite.SpriteTexture = BackgroundTexture;
	TheSprite.Options.Position(ActualPosition).NinePatch(true, BackgroundTextureRect).Scale(SizeValue);
	TheSprite.Draw(Renderer);

	TheSprite.SpriteTexture = HandleTexture;

	TheSprite.Options.Position(ActualPosition + (Vertical ? Vector2((float)Padding, StepOffset + Padding) - BackgroundTextureRect.Position() :
		Vector2(StepOffset + Padding, (float)Padding) - BackgroundTextureRect.Position())).NinePatch(true, HandleTextureRect)
		.Scale(Vertical ? Vector2(SizeValue.x - Padding * 2 + BackgroundTextureRect.Left + BackgroundTextureRect.Right, (float)MinSize) :
		Vector2((float)MinSize, SizeValue.y - Padding * 2 + BackgroundTextureRect.Top + BackgroundTextureRect.Bottom));

	TheSprite.Draw(Renderer);
};

void UITooltip::PerformLayout()
{
	sf::String Text = (Source ? Source->GetTooltipText() : OverrideText);

	Vector2 ActualFontSize = MeasureTextSimple(Text, Manager->GetDefaultFont(), FontSize).ToFullSize();

	SizeValue = Vector2(ActualFontSize.x + 10, ActualFontSize.y + 10);
};

void UITooltip::Update(const Vector2 &ParentPosition)
{
	PerformLayout();
};

void UITooltip::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	SuperSmartPointer<UIPanel> MouseOverElement = GetManager()->GetMouseOverElement();

	if((Source.Get() == NULL || MouseOverElement != Source) && OverrideText.getSize() == 0)
		return;

	if(OverrideText.getSize() && Source.Get() == NULL && MouseOverElement.Get())
		return;

	sf::String Text = (Source ? Source->GetTooltipText() : OverrideText);

	Vector2 ActualFontSize = MeasureTextSimple(Text, Manager->GetDefaultFont(), FontSize).ToFullSize();

	Vector2 ActualPosition = ParentPosition + PositionValue;

	ActualPosition.y -= SizeValue.y;

	if(ActualPosition.x > Renderer->getSize().x / 2)
		ActualPosition.x -= SizeValue.x;

	if(AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x ||
		ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->getSize().y))
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glBindTexture(GL_TEXTURE_2D, NULL);

	Vector2 Vertices[6] = {
		ActualPosition,
		ActualPosition + Vector2(0, SizeValue.y),
		ActualPosition + SizeValue,
		ActualPosition + SizeValue,
		ActualPosition + Vector2(SizeValue.x, 0),
		ActualPosition,
	};

	glColor4f(0.98f, 0.96f, 0.815f, 1);

	glVertexPointer(2, GL_FLOAT, 0, Vertices);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glColor4f(1, 1, 1, 1);

	if(!ParentValue)
		glDisableClientState(GL_VERTEX_ARRAY);

	RenderTextSimple(*Renderer, Text, Manager->GetDefaultFont(), FontSize,
		Vector4(0, 0, 0, 1), ActualPosition + (SizeValue - ActualFontSize) / 2);
};

void UIWindow::CheckClosePressed(UIPanel *This)
{
	Vector2 ActualPosition = PositionValue - Vector2((float)Padding, 0) + GetParentPosition();

	Vector2 ActualSize = SizeValue + Vector2((float)Padding * 2, TextureRect.Top);

	AxisAlignedBoundingBox AABB;
	AABB.min = ActualPosition + Vector2(ActualSize.x - CloseButtonTexture->getSize().x - CloseButtonPosition.x,
		CloseButtonPosition.y);
	AABB.max = AABB.min + Vector2(CloseButtonTexture->getSize().x, CloseButtonTexture->getSize().y);

	if(AABB.IsInside(GetManager()->GetInput()->MousePosition))
	{
		Closed = true;
		SetVisible(false);
	};
};

void UIWindow::OnSkinChange()
{
	std::string TexturePath = Skin->GetString("Window", "BackgroundTexture");

	BackgroundTexture = LoadTexture(TexturePath);

	std::string NinePatchRectValue = Skin->GetString("Window", "TextureRect");

	sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
		&TextureRect.Top, &TextureRect.Bottom);

	std::string PaddingValue = Skin->GetString("Window", "Padding");

	sscanf(PaddingValue.c_str(), "%u", &Padding);

	std::string TitleBarHeightValue = Skin->GetString("Window", "TitleBarHeight");

	sscanf(TitleBarHeightValue.c_str(), "%u", &TitleHeight);

	std::string TitleBarOffsetValue = Skin->GetString("Window", "TitleBarOffset");

	sscanf(TitleBarOffsetValue.c_str(), "%u", &TitleOffset);

	std::string TitlePositionValue = Skin->GetString("Window", "TitlePosition");

	sscanf(TitlePositionValue.c_str(), "%f,%f", &TitlePosition.x, &TitlePosition.y);

	std::string TitleFontSizeValue = Skin->GetString("Window", "TitleFontSize");

	sscanf(TitleFontSizeValue.c_str(), "%u", &TitleFontSize);

	std::string TitleFontColorValue = Skin->GetString("Window", "TitleFontColor");

	sscanf(TitleFontColorValue.c_str(), "%f,%f,%f,%f", &TitleFontColor.x, &TitleFontColor.y, &TitleFontColor.z, &TitleFontColor.w);

	std::string CloseButtonPositionValue = Skin->GetString("Window", "CloseButtonPosition");

	sscanf(CloseButtonPositionValue.c_str(), "%f,%f", &CloseButtonPosition.x, &CloseButtonPosition.y);

	TexturePath = Skin->GetString("Window", "CloseButtonTexture");

	CloseButtonTexture = LoadTexture(TexturePath);

	SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
};

void UIWindow::PerformLayout()
{
	for(unsigned long i = 0; i < Children.size(); i++)
	{
		Children[i]->PerformLayout();
	};
};

void UIWindow::Update(const Vector2 &ParentPosition)
{
	if(Closed)
		return;

	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!GetManager()->GetInput()->MouseButtons[sf::Mouse::Left].Pressed)
	{
		Dragging = false;
	}
	else if(GetManager()->GetInput()->MouseButtons[sf::Mouse::Left].JustPressed &&
		Manager->GetFocusedElement().Get() == this)
	{
		Vector2 ActualSize = SizeValue + Vector2((float)Padding * 2, TextureRect.Top);

		AxisAlignedBoundingBox AABB;
		AABB.min = ActualPosition + Vector2(0, (float)TitleOffset);
		AABB.max = AABB.min + Vector2(ActualSize.x, (float)TitleHeight);

		if(AABB.IsInside(GetManager()->GetInput()->MousePosition))
		{
			Dragging = true;
			LastMousePosition = GetManager()->GetInput()->MousePosition;
		};
	}
	else if(Dragging)
	{
		Vector2 Difference = GetManager()->GetInput()->MousePosition - LastMousePosition;

		ActualPosition += Difference;
		PositionValue += Difference;

		if(Manager->GetCurrentMenuBar() && GetParentPosition().y + PositionValue.y < 30)
		{
			PositionValue.y = ActualPosition.y = 30;
		};

		float X = GetParentPosition().x + PositionValue.x;

		if(X < 0)
		{
			PositionValue.x = ActualPosition.x = -X;
		};

		LastMousePosition = GetManager()->GetInput()->MousePosition;
	};

	for(unsigned long i = 0; i < Children.size(); i++)
	{
		Children[i]->Update(ActualPosition);
	};
};

void UIWindow::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;
	Vector2 ActualSize = SizeValue + Vector2((float)Padding * 2, TextureRect.Top);

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x || ActualPosition.y + SizeValue.y < 0 ||
		ActualPosition.y > Renderer->getSize().y))
		return;

	Sprite TheSprite;
	TheSprite.SpriteTexture = BackgroundTexture;
	TheSprite.Options.Position(ActualPosition - Vector2((float)Padding, 0)).NinePatch(true, TextureRect).Scale(ActualSize);

	TheSprite.Draw(Renderer);

	RenderTextSimple(*Renderer, Title, Manager->GetDefaultFont(), TitleFontSize,
		TitleFontColor, Vector2(ActualPosition + TitlePosition - Vector2((float)Padding, 0)));

	Sprite CloseButtonSprite;
	CloseButtonSprite.SpriteTexture = CloseButtonTexture;
	CloseButtonSprite.Options.Position(ActualPosition - Vector2((float)Padding, 0) + Vector2(ActualSize.x - CloseButtonTexture->getSize().x -
		CloseButtonPosition.x, CloseButtonPosition.y));
	CloseButtonSprite.Draw(Renderer);

	for(unsigned long i = 0; i < Children.size(); i++)
	{
		Children[i]->Draw(ActualPosition, Renderer);
	};
};

bool UIWindow::IsClosed()
{
	return Closed;
};

void UIWindow::SetClosed(bool Value)
{
	Closed = Value;
	SetVisible(!Closed);
};

void UICheckBox::OnSkinChange()
{
	std::string TexturePath = Skin->GetString("CheckBox", "CheckTexture");

	CheckTexture = LoadTexture(TexturePath);

	TexturePath = Skin->GetString("CheckBox", "UnCheckTexture");

	UnCheckTexture = LoadTexture(TexturePath);

	FontSize = Manager->GetDefaultFontSize();
	FontColor = Manager->GetDefaultFontColor();

	std::string LabelOffsetValue = Skin->GetString("CheckBox", "LabelOffset");

	sscanf(LabelOffsetValue.c_str(), "%f,%f", &LabelOffset.x, &LabelOffset.y);
};

void UICheckBox::CheckMouseClick(UIPanel *Self)
{
	Checked = !Checked;
};

void UICheckBox::PerformLayout()
{
	SizeValue = Vector2((CheckTexture.Get() ? (float)CheckTexture->getSize().x : 0.f) + MeasureTextSimple(Caption,
		Manager->GetDefaultFont(), FontSize).ToFullSize().x + LabelOffset.x, CheckTexture.Get() ? (float)CheckTexture->getSize().y : 0.f);
};

void UICheckBox::Update(const Vector2 &ParentPosition)
{
	PerformLayout();
};

void UICheckBox::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x || ActualPosition.y + SizeValue.y < 0 ||
		ActualPosition.y > Renderer->getSize().y))
		return;

	Sprite TheSprite;
	TheSprite.SpriteTexture = Checked ? CheckTexture : UnCheckTexture;
	TheSprite.Options = SpriteDrawOptions().Position(ActualPosition);
	TheSprite.Draw(Renderer);

	RenderTextSimple(*Renderer, Caption, Manager->GetDefaultFont(), FontSize, FontColor, ActualPosition +
		Vector2((float)TheSprite.SpriteTexture->getSize().x, 0) + LabelOffset);
};

void UIMenu::OnSkinChange()
{
	std::string TexturePath = Skin->GetString("Menu", "BackgroundTexture");

	BackgroundTexture = LoadTexture(TexturePath);

	TexturePath = Skin->GetString("Menu", "SelectorBackgroundTexture");

	SelectorBackgroundTexture = LoadTexture(TexturePath);

	std::string NinePatchRectValue = Skin->GetString("Menu", "TextureRect");

	sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
		&TextureRect.Top, &TextureRect.Bottom);

	std::string PaddingValue = Skin->GetString("Menu", "Padding");

	sscanf(PaddingValue.c_str(), "%f,%f", &Padding.x, &Padding.y);

	std::string SelectorPaddingValue = Skin->GetString("Menu", "SelectorPadding");

	sscanf(SelectorPaddingValue.c_str(), "%u", &SelectorPadding);

	std::string TextOffsetValue = Skin->GetString("Menu", "TextOffset");

	sscanf(TextOffsetValue.c_str(), "%f,%f", &TextOffset.x, &TextOffset.y);

	std::string ItemHeightValue = Skin->GetString("Menu", "ItemHeight");

	sscanf(ItemHeightValue.c_str(), "%u", &ItemHeight);

	FontSize = Manager->GetDefaultFontSize();
	FontColor = Manager->GetDefaultFontColor();

	SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
};

void UIMenu::CloseSelf(UIPanel *Self)
{
	Future::Instance.Post(Manager, &UIManager::RemoveMenuFuture);
};

void UIMenu::PerformLayout()
{
	SizeValue.y = (float)Items.size() * ItemHeight + Padding.y * 2.f;

	for(unsigned long i = 0; i < Items.size(); i++)
	{
		float SizeW = MeasureTextSimple(Items[i].Caption, Manager->GetDefaultFont(), FontSize).ToFullSize().x;

		if(SizeW > SizeValue.x)
			SizeValue.x = SizeW;
	};
};

void UIMenu::OnItemClick(UIPanel *Self)
{
	Vector2 ActualPosition = PositionValue + GetParentPosition();

	const Vector2 &MousePosition = GetManager()->GetInput()->MousePosition;

	AxisAlignedBoundingBox AABB;

	for(unsigned long i = 0, ypos = 0; i < Items.size(); i++, ypos += ItemHeight)
	{
		AABB.min = ActualPosition + Vector2(Padding.x, Padding.y + (float)ypos);
		AABB.max = AABB.min + Vector2(SizeValue.x, (float)ItemHeight);

		if(AABB.IsInside(MousePosition))
		{
			OnItemSelected(&Items[i]);

			break;
		};
	};

	CloseSelf(NULL);
};

void UIMenu::Update(const Vector2 &ParentPosition)
{
	PerformLayout();
};

void UIMenu::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue - Vector2(Padding.x, 0);
	Vector2 ActualSize = SizeValue + Vector2(4 + Padding.x, Padding.y);

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x || ActualPosition.y + SizeValue.y < 0 ||
		ActualPosition.y > Renderer->getSize().y))
		return;

	Sprite TheSprite;
	TheSprite.SpriteTexture = BackgroundTexture;
	TheSprite.Options.Position(ActualPosition).NinePatch(true, TextureRect).Scale(ActualSize);

	TheSprite.Draw(Renderer);

	const Vector2 &MousePosition = GetManager()->GetInput()->MousePosition;

	AxisAlignedBoundingBox AABB;
	bool DrewSelector = false;

	for(unsigned long i = 0, ypos = 0; i < Items.size(); i++, ypos += ItemHeight)
	{
		AABB.min = ActualPosition + Vector2(Padding.x, Padding.y + (float)ypos);
		AABB.max = AABB.min + Vector2(SizeValue.x, (float)ItemHeight);

		if(!DrewSelector && AABB.IsInside(MousePosition))
		{
			DrewSelector = true;

			TheSprite.SpriteTexture = SelectorBackgroundTexture;
			TheSprite.Options.Position(AABB.min.ToVector2()).NinePatch(true, Rect()).Scale(Vector2(SizeValue.x, (float)ItemHeight + SelectorPadding));

			TheSprite.Draw(Renderer);
		};

		Vector2 TextSize = MeasureTextSimple(Items[i].Caption, Manager->GetDefaultFont(), FontSize).ToFullSize();

		RenderTextSimple(*Renderer, Items[i].Caption, Manager->GetDefaultFont(), FontSize, FontColor,
			AABB.min.ToVector2() + Vector2(0, (ItemHeight - TextSize.y) / 2.f));
	};
};

void UIMenuBar::OnSkinChange()
{
	std::string TexturePath = Skin->GetString("Menu", "SelectorBackgroundTexture");

	SelectorBackgroundTexture = LoadTexture(TexturePath);

	std::string NinePatchRectValue = Skin->GetString("Menu", "TextureRect");

	sscanf(NinePatchRectValue.c_str(), "%f,%f,%f,%f", &TextureRect.Left, &TextureRect.Right,
		&TextureRect.Top, &TextureRect.Bottom);

	std::string PaddingValue = Skin->GetString("Menu", "Padding");

	sscanf(PaddingValue.c_str(), "%f,%f", &Padding.x, &Padding.y);

	std::string SelectorPaddingValue = Skin->GetString("Menu", "SelectorPadding");

	sscanf(SelectorPaddingValue.c_str(), "%u", &SelectorPadding);

	std::string TextOffsetValue = Skin->GetString("Menu", "TextOffset");

	sscanf(TextOffsetValue.c_str(), "%f,%f", &TextOffset.x, &TextOffset.y);

	FontSize = Manager->GetDefaultFontSize();
	FontColor = Manager->GetDefaultFontColor();

	SelectBoxExtraSize = Vector2(TextureRect.Left + TextureRect.Right, TextureRect.Top + TextureRect.Bottom);
};

void UIMenuBar::OnItemSelected(UIMenu::Item *Item)
{
	OnMenuItemSelected(Item->Caption);
};

void UIMenuBar::OnItemClick(UIPanel *Self)
{
	Vector2 ActualPosition = PositionValue + GetParentPosition();
	Vector2 CurrentPosition;
	AxisAlignedBoundingBox AABB;
	bool DrewSelector = false;
	const Vector2 &MousePosition = GetManager()->GetInput()->MousePosition;

	for(unsigned long i = 0; i < Items.size(); i++)
	{
		Vector2 TextSize = MeasureTextSimple(Items[i].Caption, Manager->GetDefaultFont(), FontSize).ToFullSize();

		AABB.min = ActualPosition + CurrentPosition + Vector2(Padding.x, 0);
		AABB.max = AABB.min + Vector2(TextSize.x, 22);

		if(AABB.IsInside(MousePosition) && Items[i].SubItems.size())
		{
			UIMenu *Menu = Manager->CreateMenu(Vector2(AABB.min.x, AABB.max.y));

			for(unsigned long j = 0; j < Items[i].SubItems.size(); j++)
			{
				Menu->AddItem(Items[i].SubItems[j]);
			};

			Menu->OnItemSelected.Connect(this, &UIMenuBar::OnItemSelected);
		};

		CurrentPosition.x += TextSize.x + 5;
	};
};

void UIMenuBar::PerformLayout()
{
	SizeValue = Vector2((float)Manager->GetOwner()->getSize().x, 25);
};

void UIMenuBar::Update(const Vector2 &ParentPosition)
{
	PerformLayout();
};

void UIMenuBar::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x || ActualPosition.y + SizeValue.y < 0 ||
		ActualPosition.y > Renderer->getSize().y))
		return;

	glBindTexture(GL_TEXTURE_2D, NULL);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	Vector2 Vertices[6] = {
		Vector2(),
		Vector2(0, SizeValue.y),
		SizeValue,
		SizeValue,
		Vector2(SizeValue.x, 0),
		Vector2()
	};

	glColor4f(0, 0, 0, 1);

	glVertexPointer(2, GL_FLOAT, 0, Vertices);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glColor4f(1, 1, 1, 1);

	Vector2 CurrentPosition;

	AxisAlignedBoundingBox AABB;
	bool DrewSelector = false;
	const Vector2 &MousePosition = GetManager()->GetInput()->MousePosition;

	for(unsigned long i = 0; i < Items.size(); i++)
	{
		Vector2 TextSize = MeasureTextSimple(Items[i].Caption, Manager->GetDefaultFont(), FontSize).ToFullSize();

		AABB.min = ActualPosition + CurrentPosition + Vector2(Padding.x, 0);
		AABB.max = AABB.min + Vector2(TextSize.x, 22);

		if(!DrewSelector && AABB.IsInside(MousePosition))
		{
			DrewSelector = true;

			Sprite TheSprite;
			TheSprite.SpriteTexture = SelectorBackgroundTexture;
			TheSprite.Options.Position(AABB.min.ToVector2()).NinePatch(true, TextureRect).Scale(Vector2(TextSize.x, 22.f + SelectorPadding));

			TheSprite.Draw(Renderer);
		};

		RenderTextSimple(*Renderer, Items[i].Caption, Manager->GetDefaultFont(), FontSize, FontColor,
			AABB.min.ToVector2() + Vector2(0, (22 - 12) / 2.f));

		CurrentPosition.x += TextSize.x + 5;
	};
};

void UIMessageBox::OnMainButtonClicked(UIPanel *Self)
{
	OnButtonPressed(this, 0);

	unsigned long ManagerID = (unsigned long)Manager;

	std::vector<unsigned char> Stream(sizeof(StringID) + sizeof(unsigned long));
	memcpy(&Stream[0], &ID, sizeof(StringID));
	memcpy(&Stream[sizeof(StringID)], &ManagerID, sizeof(unsigned long));

	Future::Instance.Post(RemoveElementFuture, Stream);
};

void UIMessageBox::OnSecondaryButtonClicked(UIPanel *Self)
{
	OnButtonPressed(this, 1);

	unsigned long ManagerID = (unsigned long)Manager;

	std::vector<unsigned char> Stream(sizeof(StringID) + sizeof(unsigned long));
	memcpy(&Stream[0], &ID, sizeof(StringID));
	memcpy(&Stream[sizeof(StringID)], &ManagerID, sizeof(unsigned long));

	Future::Instance.Post(RemoveElementFuture, Stream);
};

void UIMessageBox::InitMessageBox()
{
	SetSkin(Manager->GetSkin());

	TheWindow.Reset(new UIWindow(Manager));
	TheText.Reset(new UIText(Manager));

	SuperSmartPointer<UIButton> TheMainButton(new UIButton(Manager));
	SuperSmartPointer<UIButton> TheSecondaryButton;

	TheWindow->Title = Title;
	TheWindow->SetSkin(Skin);

	TheMainButton->SetSkin(Skin);
	TheMainButton->SetSize(Vector2(100, 0));
	TheMainButton->Caption = ButtonTitle.getSize() ? ButtonTitle : "OK";
	TheMainButton->PerformLayout();
	TheMainButton->OnClick.Connect(this, &UIMessageBox::OnMainButtonClicked);

	if(SecondaryButtonTitle.getSize())
	{
		TheSecondaryButton.Reset(new UIButton(Manager));
		TheSecondaryButton->SetSkin(Skin);
		TheSecondaryButton->SetSize(Vector2(100, 0));
		TheSecondaryButton->Caption = SecondaryButtonTitle.getSize() ? SecondaryButtonTitle : "Cancel";
		TheSecondaryButton->PerformLayout();
		TheSecondaryButton->OnClick.Connect(this, &UIMessageBox::OnSecondaryButtonClicked);
	};

	Vector2 MaxSize = Vector2(Manager->GetOwner()->getSize().x * 0.95f, Manager->GetOwner()->getSize().y * 0.45f);

	TheText->SetSkin(Skin);
	TheText->SetSize(MaxSize);
	TheText->SetText(Message);

	MaxSize = TheText->GetTextSize();

	TheText->SetSize(MaxSize);
	TheWindow->SetSize(MaxSize + Vector2(0, (float)TheWindow->GetTitleHeight() + TheWindow->GetTitleOffset()));
	TheText->SetPosition(Vector2(0, (float)TheWindow->GetTitleHeight() + TheWindow->GetTitleOffset()));

	TheMainButton->SetPosition(TheText->GetPosition() + Vector2(TheSecondaryButton.Get() ? (TheWindow->GetSize().x / 2 - TheMainButton->GetSize().x * 2) :
		(TheWindow->GetSize().x - TheMainButton->GetSize().x) / 2.f, TheText->GetSize().y + 10));

	AddChildren(TheWindow);
	AddChildren(TheText);
	AddChildren(TheMainButton);

	std::string Base = std::string("__MESSAGEBOX__") + StringUtils::MakeIntString((long)this, true);

	Manager->AddElement(MakeStringID(Base + "_WINDOW__"), TheWindow);
	Manager->AddElement(MakeStringID(Base + "_TEXT__"), TheText);
	Manager->AddElement(MakeStringID(Base + "_MAINBUTTON__"), TheMainButton);

	if(TheSecondaryButton.Get())
	{
		TheSecondaryButton->SetPosition(TheText->GetPosition() + Vector2((TheWindow->GetSize().x / 2 + TheSecondaryButton->GetSize().x), TheText->GetSize().y + 10));

		AddChildren(TheSecondaryButton);

		Manager->AddElement(MakeStringID(Base + "_SECONDARYBUTTON__"), TheSecondaryButton);
	};

	PositionValue = Vector2(((float)Manager->GetOwner()->getSize().x - MaxSize.x) / 2.f,
		((float)Manager->GetOwner()->getSize().y - MaxSize.y) / 2.f);
};

void UIMessageBox::PerformLayout()
{
	SizeValue = GetChildrenSize();
};

void UIMessageBox::Update(const Vector2 &ParentPosition)
{
	PerformLayout();

	Children[0]->SetSize(SizeValue);

	for(unsigned long i = 0; i < Children.size(); i++)
	{
		Children[i]->Update(ParentPosition + PositionValue);
	};

	PositionValue = PositionValue + Children[0]->GetPosition();
	Children[0]->SetPosition(Vector2());
};

void UIMessageBox::Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
{
	Vector2 ActualPosition = ParentPosition + PositionValue;

	if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
		ActualPosition.x > Renderer->getSize().x || ActualPosition.y + SizeValue.y < 0 ||
		ActualPosition.y > Renderer->getSize().y))
		return;

	if(TheWindow->IsClosed())
	{
		unsigned long ManagerID = (unsigned long)Manager;

		std::vector<unsigned char> Stream(sizeof(StringID) + sizeof(unsigned long));
		memcpy(&Stream[0], &ID, sizeof(StringID));
		memcpy(&Stream[sizeof(StringID)], &ManagerID, sizeof(unsigned long));

		Future::Instance.Post(RemoveElementFuture, Stream);
	};

	for(unsigned long i = 0; i < Children.size(); i++)
	{
		Children[i]->Draw(ParentPosition + PositionValue, Renderer);
	};
};

void UIDraggable::OnMouseJustPressedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o)
{
	if(o.Name != sf::Mouse::Left)
		return;

	Dragging = true;

	OnDragBegin(this);
};

void UIDraggable::OnMouseReleasedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o)
{
	if(o.Name != sf::Mouse::Left)
		return;

	if(Dragging)
	{
		OnDragEnd(this);

		Dragging = false;

		SuperSmartPointer<UIDroppable> Drop = GetManager()->GetMouseOverElement();

		if(!Drop.Get())
			return;

		Drop->OnDrop(this);
	};
};

SuperSmartPointer<UIPanel> UIManager::GetMouseOverElement()
{
	UIPanel *FoundElement = NULL;

	SuperSmartPointer<UIPanel> InputBlocker;

	for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
	{
		while(it != Elements.end() && it->second.Get() == NULL)
		{
			it = Elements.erase(it);
		};

		if(it == Elements.end())
			break;

		if(it->second->Panel->BlockingInput)
		{
			InputBlocker = it->second->Panel;

			break;
		};
	};

	if(InputBlocker.Get())
	{
		UIPanel *p = InputBlocker;
		RecursiveFindFocusedElement(Vector2(), p, FoundElement);
	}
	else
	{
		for(long i = DrawOrderCounter; i >= 0; i--)
		{
			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Panel.Get() == NULL || !it->second->Panel->MouseInputValue ||
					it->second->Panel->GetParent() != NULL || it->second->DrawOrder != i)
					continue;

				UIPanel *p = it->second->Panel;

				RecursiveFindFocusedElement(Vector2(), p, FoundElement);

				if(FoundElement)
					break;
			};

			if(FoundElement)
				break;
		};
	};

	if(FoundElement && Elements[FoundElement->ID].Get())
	{
		return Elements[FoundElement->ID]->Panel;
	};

	return SuperSmartPointer<UIPanel>();
};

void UIManager::Update()
{
	for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
	{
		if(it->second.Get() == NULL || it->second->Panel.Get() == NULL)
		{
			Elements.erase(it);

			return;
		};

		if(it->second->Panel->GetParent() == NULL && it->second->Panel->IsVisible())
		{
			it->second->Panel->Update(Vector2());
		};
	};
};

void UIManager::Draw(sf::RenderWindow *Renderer)
{
	SuperSmartPointer<UIPanel> InputBlocker;

	for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
	{
		if(it->second.Get() == NULL || it->second->Panel.Get() == NULL)
		{
			Elements.erase(it);

			return;
		};

		if(it->second->Panel->BlockingInput)
		{
			InputBlocker = it->second->Panel;

			break;
		};
	};

	for(unsigned long i = 0; i <= DrawOrderCounter; i++)
	{
		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second.Get() == NULL || it->second->Panel.Get() == NULL)
			{
				Elements.erase(it);

				return;
			};

			if(it->second->DrawOrder == i && it->second->Panel->GetParent() == NULL &&
				it->second->Panel->IsVisible())
			{
				if(it->second->Panel == InputBlocker)
				{
					glBindTexture(GL_TEXTURE_2D, NULL);
					glEnableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);

					Vector2 Vertices[6] = {
						Vector2(),
						Vector2(0, Renderer->getSize().y),
						Vector2(Renderer->getSize().x, Renderer->getSize().y),
						Vector2(Renderer->getSize().x, Renderer->getSize().y),
						Vector2(Renderer->getSize().x, 0),
						Vector2(),
					};

					glVertexPointer(2, GL_FLOAT, 0, Vertices);

					glColor4f(0, 0, 0, 0.3f);

					glDrawArrays(GL_TRIANGLES, 0, 6);

					glColor4f(1, 1, 1, 1);
				};

				it->second->Panel->Draw(Vector2(), Renderer);
			};
		};
	};

	Tooltip->Update(Vector2());
	Tooltip->Draw(GetInput()->MousePosition, Renderer);
};

void UIManager::RecursiveFindFocusedElement(const Vector2 &ParentPosition, UIPanel *p, UIPanel *&FoundElement)
{
	if(!p->IsVisible() || !p->IsEnabled() || !p->IsMouseInputEnabled())
		return;

	static AxisAlignedBoundingBox AABB;

	AABB.min = ParentPosition + p->GetPosition() - p->SelectBoxExtraSize / 2;
	AABB.max = AABB.min + p->GetSize() + p->SelectBoxExtraSize;

	if(AABB.IsInside(GetInput()->MousePosition) &&
		p->MouseInputValue)
	{
		FoundElement = p;

		for(unsigned long i = 0; i < p->Children.size(); i++)
		{
			RecursiveFindFocusedElement(ParentPosition + p->GetPosition() - p->GetTranslation() *
				(p->GetParent() == NULL ? p->GetSize() : p->SizeValue - p->GetParent()->GetSize()),
				p->Children[i], FoundElement);
		};
	};
};

void UIManager::OnMouseJustPressedPriv(const InputCenter::MouseButtonInfo &o)
{
	if(o.Name == sf::Mouse::Left)
	{
		SuperSmartPointer<UIPanel> PreviouslyFocusedElement = FocusedElementValue;
		FocusedElementValue = SuperSmartPointer<UIPanel>();
		UIPanel *FoundElement = NULL;

		SuperSmartPointer<UIPanel> InputBlocker;

		for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
		{
			if(it->second->Panel->BlockingInput)
			{
				InputBlocker = it->second->Panel;

				break;
			};
		};

		if(InputBlocker.Get())
		{
			UIPanel *p = InputBlocker;
			RecursiveFindFocusedElement(Vector2(), p, FoundElement);
		}
		else
		{
			for(long i = DrawOrderCounter; i >= 0; i--)
			{
				for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
				{
					if(it->second->Panel.Get() == NULL || !it->second->Panel->MouseInputValue ||
						it->second->Panel->GetParent() != NULL || it->second->DrawOrder != i)
						continue;

					UIPanel *p = it->second->Panel;

					RecursiveFindFocusedElement(Vector2(), p, FoundElement);

					if(FoundElement)
						break;
				};

				if(FoundElement)
					break;
			};
		};

		if(FoundElement)
		{
			FocusedElementValue = Elements[FoundElement->ID]->Panel;
		};

		if(PreviouslyFocusedElement && PreviouslyFocusedElement.Get() != FocusedElementValue.Get())
		{
			PreviouslyFocusedElement->OnLoseFocusPriv();
		};

		if(FoundElement)
		{
			for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
			{
				if(it->second->Panel.Get() == FoundElement)
				{
					FocusedElementValue = it->second->Panel;
				};
			};
		};

		if(FocusedElementValue)
		{
			FocusedElementValue->OnGainFocusPriv();
		};

		if(CurrentMenu.Get() && FocusedElementValue.Get() != CurrentMenu.Get())
		{
			RemoveMenuFuture(std::vector<unsigned char>());
		};
	};

	if(FocusedElementValue)
	{
		FocusedElementValue->OnMouseJustPressedPriv(o);
	};
};

void UIManager::OnMousePressedPriv(const InputCenter::MouseButtonInfo &o)
{
	if(FocusedElementValue)
	{
		FocusedElementValue->OnMousePressedPriv(o);
	};
};

void UIManager::OnMouseReleasedPriv(const InputCenter::MouseButtonInfo &o)
{
	if(FocusedElementValue)
	{
		FocusedElementValue->OnMouseReleasedPriv(o);
	};
};

void UIManager::OnMouseMovePriv()
{
	if(FocusedElementValue)
	{
		FocusedElementValue->OnMouseMovePriv();
	};

	SuperSmartPointer<UIPanel> SelectedElement = GetMouseOverElement();

	if(SelectedElement.Get() == NULL)
	{
		if(Tooltip->Source)
		{
			Tooltip->Source = SuperSmartPointer<UIPanel>();
		}
	}
	else if(SelectedElement->RespondsToTooltips())
	{
		Tooltip->Source = SelectedElement;
	};
};

void UIManager::OnKeyJustPressedPriv(const InputCenter::KeyInfo &o)
{
	if(FocusedElementValue)
	{
		FocusedElementValue->OnKeyJustPressedPriv(o);
	};
};

void UIManager::OnKeyPressedPriv(const InputCenter::KeyInfo &o)
{
	if(FocusedElementValue)
	{
		FocusedElementValue->OnKeyPressedPriv(o);
	};
};

void UIManager::OnKeyReleasedPriv(const InputCenter::KeyInfo &o)
{
	if(FocusedElementValue)
	{
		FocusedElementValue->OnKeyReleasedPriv(o);
	};
};

void UIManager::OnCharacterEnteredPriv()
{
	if(FocusedElementValue)
	{
		FocusedElementValue->OnCharacterEnteredPriv();
	};
};

void UIManager::RegisterInput()
{
	for(unsigned long i = 0; i < sf::Keyboard::KeyCount; i++)
	{
		GetInput()->Keys[i].OnJustPressed.Connect(this, &UIManager::OnKeyJustPressedPriv);
		GetInput()->Keys[i].OnPressed.Connect(this, &UIManager::OnKeyPressedPriv);
		GetInput()->Keys[i].OnReleased.Connect(this, &UIManager::OnKeyReleasedPriv);
	};

	for(unsigned long i = 0; i < sf::Mouse::ButtonCount; i++)
	{
		GetInput()->MouseButtons[i].OnJustPressed.Connect(this, &UIManager::OnMouseJustPressedPriv);
		GetInput()->MouseButtons[i].OnPressed.Connect(this, &UIManager::OnMousePressedPriv);
		GetInput()->MouseButtons[i].OnReleased.Connect(this, &UIManager::OnMouseReleasedPriv);
	};

	GetInput()->OnCharacterEntered.Connect(this, &UIManager::OnCharacterEnteredPriv);
	GetInput()->OnMouseMove.Connect(this, &UIManager::OnMouseMovePriv);
};

void UIManager::UnRegisterInput()
{
	for(unsigned long i = 0; i < sf::Keyboard::KeyCount; i++)
	{
		GetInput()->Keys[i].OnJustPressed.Disconnect(this, &UIManager::OnKeyJustPressedPriv);
		GetInput()->Keys[i].OnPressed.Disconnect(this, &UIManager::OnKeyPressedPriv);
		GetInput()->Keys[i].OnReleased.Disconnect(this, &UIManager::OnKeyReleasedPriv);
	};

	for(unsigned long i = 0; i < sf::Mouse::ButtonCount; i++)
	{
		GetInput()->MouseButtons[i].OnJustPressed.Disconnect(this, &UIManager::OnMouseJustPressedPriv);
		GetInput()->MouseButtons[i].OnPressed.Disconnect(this, &UIManager::OnMousePressedPriv);
		GetInput()->MouseButtons[i].OnReleased.Disconnect(this, &UIManager::OnMouseReleasedPriv);
	};

	GetInput()->OnCharacterEntered.Disconnect(this, &UIManager::OnCharacterEnteredPriv);
	GetInput()->OnMouseMove.Disconnect(this, &UIManager::OnMouseMovePriv);
};

bool UIManager::AddElement(StringID ID, SuperSmartPointer<UIPanel> Element)
{
	ElementMap::iterator it = Elements.find(ID);

	if(it != Elements.end())
	{
		if(!it->second.Get() || !it->second->Panel.Get())
		{
			Elements.erase(it);
		}
		else
		{
			return false;
		};
	};

	if(Element->Manager != this)
		return false;

	Elements[ID].Reset(new ElementInfo());
	Elements[ID]->Panel = Element;
	Elements[ID]->DrawOrder = ++DrawOrderCounter;
	Element->ID = ID;
	Element->SetSkin(Skin);

	return true;
};

void UIManager::RemoveElement(StringID ID)
{
	ElementMap::iterator it = Elements.find(ID);

	if(it != Elements.end())
	{
		it->second.Dispose();

		Elements.erase(it);
	};
};

void UIManager::Clear()
{
	while(Elements.begin() != Elements.end())
	{
		Elements.begin()->second.Dispose();
		Elements.erase(Elements.begin());
	};

	FocusedElementValue.Dispose();
};

SuperSmartPointer<UIPanel> UIManager::GetElement(StringID ID)
{
	ElementMap::iterator it = Elements.find(ID);

	if(it != Elements.end())
	{
		return it->second->Panel;
	};

	return SuperSmartPointer<UIPanel>();
};

SuperSmartPointer<UIPanel> UIManager::GetFocusedElement()
{
	return FocusedElementValue;
};

UITooltip &UIManager::GetTooltip()
{
	//Will always be non-null
	return *Tooltip.Get();
};

void UIManager::SetSkin(SuperSmartPointer<GenericConfig> Skin)
{
	this->Skin = Skin;

	std::string DefaultFontColorValue = Skin->GetString("General", "DefaultFontColor");

	sscanf(DefaultFontColorValue.c_str(), "%f,%f,%f,%f", &DefaultFontColor.x, &DefaultFontColor.y,
		&DefaultFontColor.z, &DefaultFontColor.w);

	std::string DefaultFontSizeValue = Skin->GetString("General", "DefaultFontSize");

	sscanf(DefaultFontSizeValue.c_str(), "%u", &DefaultFontSize);

	std::string DefaultFontValue = Skin->GetString("General", "DefaultFont");

	DefaultFont.Reset(new sf::Font());

	if(!DefaultFont->loadFromFile(DefaultFontValue))
		DefaultFont.Dispose();

	for(ElementMap::iterator it = Elements.begin(); it != Elements.end(); it++)
	{
		if(it->second.Get() == NULL)
			continue;

		it->second->Panel->SetSkin(Skin);
	};
};

UIMenu *UIManager::CreateMenu(const Vector2 &Position)
{
	if(CurrentMenu.Get())
		RemoveElement(CurrentMenu->ID);

	CurrentMenu.Reset(new UIMenu(this));
	CurrentMenu->SetPosition(Position);

	AddElement(MakeStringID("__UIMANAGER_CURRENT_MENU__"), CurrentMenu);

	return CurrentMenu;
};

UIMenuBar *UIManager::CreateMenuBar()
{
	if(CurrentMenuBar.Get())
		RemoveElement(CurrentMenuBar->ID);

	CurrentMenuBar.Dispose();
	CurrentMenuBar.Reset(new UIMenuBar(this));

	AddElement(MakeStringID("__UIMANAGER_CURRENT_MENU_BAR__"), CurrentMenuBar);

	return CurrentMenuBar;
};

void UIManager::RemoveMenuFuture(std::vector<unsigned char> &Stream)
{
	RemoveElement(MakeStringID("__UIMANAGER_CURRENT_MENU__"));
};
