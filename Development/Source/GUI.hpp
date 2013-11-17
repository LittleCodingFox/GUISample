#pragma once

class UIManager;
class UILayout;

#define FLAGVALUE(x) (1 << x)

/*!
*	Text rendering parameters
*/
class TextParams
{
public:
	Vector4 TextColorValue, SecondaryTextColorValue, BorderColorValue;
	Vector2 PositionValue;
	float BorderSizeValue;

	TextParams() : BorderSizeValue(0), TextColorValue(1, 1, 1, 1), SecondaryTextColorValue(1, 1, 1, 1), BorderColorValue(0, 0, 0, 1) {}
	TextParams(const TextParams &o) : BorderSizeValue(o.BorderSizeValue), TextColorValue(o.TextColorValue), SecondaryTextColorValue(o.SecondaryTextColorValue),
		BorderColorValue(o.BorderColorValue), PositionValue(o.PositionValue) {};

	TextParams &operator=(const TextParams &o)
	{
		BorderSizeValue = o.BorderSizeValue;
		TextColorValue = o.TextColorValue;
		SecondaryTextColorValue = o.SecondaryTextColorValue;
		BorderColorValue = o.BorderColorValue;
		PositionValue = o.PositionValue;

		return *this;
	};

	/*!
	*	Sets the text color
	*	\param Color the color to use
	*	\note Changes the secondary color too to prevent issues with using this
	*	\sa SecondaryColor
	*/
	TextParams &Color(const Vector4 &Color)
	{
		TextColorValue = SecondaryTextColorValue = Color;

		return *this;
	};

	/*!
	*	Sets the secondary text color
	*	\param SecondaryColor the secondary color to use
	*	\sa Color
	*/
	TextParams &SecondaryColor(const Vector4 &SecondaryColor)
	{
		SecondaryTextColorValue = SecondaryColor;

		return *this;
	};

	/*!
	*	Sets the border color for the text
	*	\param BorderColor the border color to use (default value is 0, 0, 0, 1)
	*	\sa BorderSize
	*/
	TextParams &BorderColor(const Vector4 &BorderColor)
	{
		BorderColorValue = BorderColor;

		return *this;
	};

	/*!
	*	Sets the border size for the text
	*	\param BorderSize the size of the border, in pixels (default value is 0)
	*	\sa BorderColor
	*/
	TextParams &BorderSize(float BorderSize)
	{
		BorderSizeValue = BorderSize;

		return *this;
	};

	/*!
	*	Sets the text position
	*	\param Position the text's position
	*/
	TextParams &Position(const Vector2 &Position)
	{
		PositionValue = Position;

		return *this;
	};
};

/*!
*	UI Panel class
*/
class UIPanel
{
	friend class UIManager;
protected:
	bool VisibleValue, EnabledValue, MouseInputValue, KeyboardInputValue;
	Vector2 PositionValue, SizeValue, TranslationValue, SelectBoxExtraSize;
	float AlphaValue;
	UILayout *Layout;
	UIManager *Manager;
	bool RespondsToTooltipsValue;
	sf::String TooltipValue;
	SuperSmartPointer<UIPanel> ParentValue;
	//!<Children of this UI Panel
	std::vector<UIPanel *> Children;
	unsigned long long ClickTimer;
	//!<Whether we are in the middle of a click
	bool ClickPressed;
	//!<Whether we are blocking Input
	bool BlockingInput;
	StringID ID;
	SuperSmartPointer<GenericConfig> Skin;

	/*!
	*	Called by the UI Manager when the UI Skin is changed
	*/
	virtual void OnSkinChange() {};

	void OnMouseJustPressedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(!MouseInputValue || !EnabledValue)
			return;

		if(o.Name == sf::Mouse::Left)
		{
			ClickTimer = time(NULL);
			ClickPressed = true;
		};

		OnMouseJustPressed(this, o);
	};

	void OnMousePressedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(!MouseInputValue || !EnabledValue)
			return;

		OnMousePressed(this, o);
	};

	void OnMouseReleasedPriv(const InputCenter::MouseButtonInfo &o)
	{
		if(!MouseInputValue || !EnabledValue)
			return;

		if(o.Name == sf::Mouse::Left)
		{
			ClickPressed = false;

			if(time(NULL) - ClickTimer < 500)
			{
				OnClick(this);
			};
		};

		OnMouseReleased(this, o);
	};

	void OnMouseMovePriv()
	{
		if(!MouseInputValue || !EnabledValue)
			return;

		OnMouseMove(this);
	};

	void OnKeyPressedPriv(const InputCenter::KeyInfo &o)
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		OnKeyPressed(this, o);
	};

	void OnKeyJustPressedPriv(const InputCenter::KeyInfo &o)
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		OnKeyJustPressed(this, o);
	};

	void OnKeyReleasedPriv(const InputCenter::KeyInfo &o)
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		OnKeyReleased(this, o);
	};

	void OnCharacterEnteredPriv()
	{
		if(!KeyboardInputValue || !EnabledValue)
			return;

		OnCharacterEntered(this);
	};

	void OnLoseFocusPriv()
	{
		OnLoseFocus(this);
	};

	void OnGainFocusPriv()
	{
		OnGainFocus(this);
	};

	/*!
	*	\note Should be called whenever the UI object is created
	*/
	void OnConstructed()
	{
		EnabledValue = MouseInputValue = KeyboardInputValue = VisibleValue = true;
		AlphaValue = 1;
		ClickPressed = BlockingInput = RespondsToTooltipsValue = false;
	};
public:

	Signal1<UIPanel *> OnClick, OnLoseFocus, OnGainFocus, OnCharacterEntered, OnMouseMove;
	Signal2<UIPanel *, const InputCenter::MouseButtonInfo &> OnMouseJustPressed, OnMousePressed, OnMouseReleased;
	Signal2<UIPanel *, const InputCenter::KeyInfo &> OnKeyJustPressed, OnKeyPressed, OnKeyReleased;

	UIPanel(UIManager *_Manager) : Manager(_Manager), VisibleValue(true),
		EnabledValue(true), MouseInputValue(true), KeyboardInputValue(true), AlphaValue(1),
		ClickPressed(false), BlockingInput(false)
	{
	};

	virtual ~UIPanel();

	/*!
	*	Updates this element based on the parent position
	*	\param ParentPosition the parent's position
	*	\note Should iterate on all children
	*/
	virtual void Update(const Vector2 &ParentPosition) = 0;
	/*!
	*	Draws this element based on the parent position
	*	\param ParentPosition the parent's position
	*	\param Renderer the Renderer to draw to
	*	\note Should iterate on all children
	*/
	virtual void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer) = 0;

	/*!
	*	\return Whether this element respondes to Tooltips
	*/
	bool RespondsToTooltips() { return RespondsToTooltipsValue && TooltipValue.getSize(); };

	/*!
	*	Sets whether this element has tooltips
	*	\param Value whether it does respond to tooltips
	*/
	void SetRespondsToTooltips(bool Value)
	{
		RespondsToTooltipsValue = Value;
	};

	/*!
	*	Perform auto calculation of sizes here
	*/
	virtual void PerformLayout()
	{
		for(unsigned long i = 0; i < Children.size(); i++)
		{
			Children[i]->PerformLayout();
		};
	};

	/*!
	*	\return the Tooltip Text to display
	*/
	const sf::String &GetTooltipText()
	{
		return TooltipValue;
	};

	/*
	*	\return whether this panel is blocking input
	*/
	bool IsBlockingInput()
	{
		return BlockingInput;
	};

	/*
	*	Sets whether this panel is blocking input
	*	\param Value whether to block input
	*/
	void SetBlockingInput(bool Value)
	{
		BlockingInput = Value;
	};

	/*!
	*	Sets the tooltip text for this element
	*	\param Text the tooltip text
	*/
	void SetTooltipText(const sf::String &Text)
	{
		TooltipValue = Text;
	};

	/*!
	*	\return the UIManager associated to this UI Element
	*	\sa UIManager
	*/
	UIManager *GetManager()
	{
		return Manager;
	};

	/*!
	*	\return the ID of this UI Element
	*/
	StringID GetID()
	{
		return ID;
	};

	/*!
	*	Calculates the position of this element and all its parents to return the overall (absolute) position
	*	\return the calculated absolute position of this UI Element
	*/
	Vector2 GetParentPosition()
	{
		if(!ParentValue)
			return Vector2();

		Vector2 Position;
		UIPanel *p = ParentValue;

		while(p)
		{
			Position += p->GetPosition() + p->GetTranslation();

			p = p->GetParent();
		};

		return Position;
	};

	/*!
	*	Set this Element's Visiblity
	*	\param value whether the element should be visible
	*/
	void SetVisible(bool value)
	{
		VisibleValue = value;
	};

	/*!
	*	\return whether this element is visible
	*/
	bool IsVisible() const
	{
		return VisibleValue;
	};

	/*!
	*	Sets this element's Enabled status
	*	\param value whether this element is enabled
	*/
	void SetEnabled(bool value)
	{
		EnabledValue = value;
	};

	/*!
	*	\return whether this element is enabled
	*/
	bool IsEnabled() const
	{
		return EnabledValue;
	};

	/*!
	*	Sets whether this element receives Mouse Input
	*	\param value whether this element receives Mouse Input
	*/
	void SetMouseInputEnabled(bool value)
	{
		MouseInputValue = value;
	};

	/*!
	*	\return whether this element receives Mouse Input
	*/
	bool IsMouseInputEnabled() const
	{
		return MouseInputValue;
	};

	/*!
	*	Sets whether this element receives Keyboard Input
	*	\param value whether this element receives Keyboard Input
	*/
	void SetKeyboardInputEnabled(bool value)
	{
		KeyboardInputValue = value;
	};

	/*!
	*	\return whether this element receivews Keyboard Input
	*/
	bool IsKeyboardInputEnabled() const
	{
		return KeyboardInputValue;
	};

	/*!
	*	Adds a children to this element
	*	\param Child the Child to add
	*/
	void AddChildren(UIPanel *Child);

	/*!
	*	Removes a children from this element
	*	\param Child the Child to remove
	*/
	void RemoveChild(UIPanel *Child)
	{
		for(std::vector<UIPanel *>::iterator it = Children.begin(); it != Children.end(); it++)
		{
			if(*it == Child)
			{
				Child->ParentValue = SuperSmartPointer<UIPanel>();
				Children.erase(it);

				return;
			};
		};
	};

	/*!
	*	\return the Translation of this element
	*	\note Used by elements such as Scrollbars to indicate how much translation there is
	*/
	const Vector2 &GetTranslation() const
	{
		return TranslationValue;
	};

	/*!
	*	\return the Parent of this Element (or NULL)
	*/
	UIPanel *GetParent() const
	{
		return (UIPanel *)ParentValue.Get();
	};

	/*!
	*	\return the Position of this Element
	*/
	const Vector2 &GetPosition() const
	{
		return PositionValue;
	};

	/*!
	*	Sets the Position of this element
	*	\param Position the new position for this element
	*/
	void SetPosition(const Vector2 &Position)
	{
		PositionValue = Position;
	};

	/*!
	*	\return the Size of this Element
	*/
	const Vector2 &GetSize() const
	{
		return SizeValue;
	};

	/*!
	*	Sets the Size of this element
	*	\param Size the new size for this element
	*/
	void SetSize(const Vector2 &Size)
	{
		SizeValue = Size;

		if(SizeValue.x < 0)
			SizeValue.x = 0;

		if(SizeValue.y < 0)
			SizeValue.y = 0;
	};

	/*!
	*	\return the Alpha Transparency of this Element
	*/
	float GetAlpha() const
	{
		return AlphaValue;
	};

	/*!
	*	Sets the Alpha Transparency of this Element
	*	\param Alpha the new alpha for this element
	*/
	void SetAlpha(float Alpha)
	{
		AlphaValue = Alpha;
	};

	/*!
	*	\return the Size of all children in this element
	*/
	Vector2 GetChildrenSize()
	{
		Vector2 Out;

		for(unsigned long i = 0; i < Children.size(); i++)
		{
			if(!Children[i]->IsVisible())
				continue;

			Children[i]->PerformLayout();

			if(Out.x < Children[i]->GetPosition().x + Children[i]->GetSize().x)
				Out.x = Children[i]->GetPosition().x + Children[i]->GetSize().x;

			if(Out.y < Children[i]->GetPosition().y + Children[i]->GetSize().y)
				Out.y = Children[i]->GetPosition().y + Children[i]->GetSize().y;
		};

		return Out;
	};

	/*!
	*	Clears all children (and destroys them)
	*/
	void Clear();

	/*!
	*	Sets the Skin of this Element
	*	\param Skin the new skin to apply
	*/
	void SetSkin(SuperSmartPointer<GenericConfig> Skin)
	{
		this->Skin = Skin;
		OnSkinChange();
	};

	/*!
	*	Gets the layout this Panel belongs to
	*	\return the Layout associated with this Panel, or NULL
	*/
	UILayout *GetLayout()
	{
		return Layout;
	};

	/*!
	*	Gets the extra size for the element
	*	Extra size is based on the Nine-Patch corner sizes
	*/
	const Vector2 &GetExtraSize()
	{
		return SelectBoxExtraSize;
	};
};

/*!
*	Generic Group Container which does nothing more than perform layout and update/draw children
*/
class UIGroup : public UIPanel
{
public:
	UIGroup(UIManager *Manager) : UIPanel(Manager)
	{
		OnConstructed();
	};

	void Update(const Vector2 &ParentPosition)
	{
		PerformLayout();

		for(unsigned long i = 0; i < Children.size(); i++)
		{
			Children[i]->Update(ParentPosition + PositionValue);
		};
	};

	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer)
	{
		Vector2 ActualPosition = ParentPosition + PositionValue;

		if(!IsVisible() || AlphaValue == 0 || (ActualPosition.x + SizeValue.x < 0 ||
			ActualPosition.x > Renderer->getSize().x ||
			ActualPosition.y + SizeValue.y < 0 || ActualPosition.y > Renderer->getSize().y))
			return;

		for(unsigned long i = 0; i < Children.size(); i++)
		{
			Children[i]->Draw(ActualPosition, Renderer);
		};
	};
};

/*!
*	UI Frame Element
*/
class UIFrame : public UIPanel
{
protected:
	SuperSmartPointer<sf::Texture> BackgroundTexture;
	Rect TextureRect;

	void OnSkinChange();
public:

	UIFrame(UIManager *Manager) : UIPanel(Manager)
	{
		OnConstructed();
	};

	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI Button Element
*	\note Size can be 0, since it'll resize itself on draw
*/
class UIButton : public UIPanel
{
protected:
	SuperSmartPointer<sf::Texture> NormalTexture, FocusedTexture;
	Rect TextureRect;
	Vector2 LabelOffset;
	Vector4 FontColor;

	void OnSkinChange();
public:
	/*!
	*	The Button's Text
	*/
	sf::String Caption;

	/*!
	*	The Button's Font Size
	*/
	unsigned long FontSize;

	UIButton(UIManager *Manager) : UIPanel(Manager), FontSize(12)
	{
		OnConstructed();
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI Text Box Element
*	\note Size can be 0 on Y, will autoresize that way
*/
class UITextBox : public UIPanel
{
protected:
	SuperSmartPointer<sf::Texture> BackgroundTexture;
	Rect TextureRect;
	sf::String Text;
	unsigned long CursorPosition, TextOffset, Padding;
	std::string FontName;
	bool IsPasswordValue;

	Vector2 LastSizeValue;

	void OnMouseJustPressedTextBox(UIPanel *This, const InputCenter::MouseButtonInfo &o);
	void OnKeyJustPressedTextBox(UIPanel *This, const InputCenter::KeyInfo &o);
	void OnCharacterEnteredTextBox(UIPanel *This);
	void OnSkinChange();
public:
	/*!
	*	The Text Box's Font Size
	*/
	unsigned long FontSize;

	UITextBox(UIManager *Manager);
	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);

	/*!
	*	\return whether this textbox is a password
	*/
	bool IsPassword()
	{
		return IsPasswordValue;
	};

	/*!
	*	Sets whether this textbox is a password
	*	\param Value whether this textbox is a password
	*/
	void SetPassword(bool Value)
	{
		IsPasswordValue = Value;
	};

	/*!
	*	\return the Text of this Text Box
	*/
	const sf::String &GetText() const
	{
		return Text;
	};

	/*!
	*	Sets the Text of this Text Box
	*	\param Text the new Text
	*/
	void SetText(const sf::String &Text)
	{
		this->Text = Text;
		CursorPosition = TextOffset = 0;
	};
};

#define SCROLLBAR_DRAGGABLE_SIZE 15

class UIScrollbar;

/*!
*	UI Scrollable Frame Element
*	\note Will not Autoresize
*	\note Includes SCROLLBAR_DRAGGABLE_SIZE Pixels on the Size's Width should scrolling be required
*/
class UIScrollableFrame : public UIPanel
{
	friend class UIList;
protected:
	SuperSmartPointer<UIScrollbar> VerticalScroll, HorizontalScroll;

	void OnSkinChange();
	void MakeScrolls();
public:
	UIScrollableFrame(UIManager *Manager) : UIPanel(Manager)
	{
		OnConstructed();
	};

	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI List Element
*	\note Will AutoSize outwards vertically
*	\note Must have a ScrollableFrame parent
*/
class UIList : public UIPanel
{
private:
	SuperSmartPointer<sf::Texture> SelectorBackgroundTexture;
	void OnSkinChange();
	void OnItemClickCheck(UIPanel *Self);
public:
	/*!
	*	All items contained by this List
	*/
	std::vector<std::string> Items;
	/*!
	*	The List's Font Size
	*/
	unsigned long FontSize;

	/*!
	*	(UIList Self, unsigned long ItemIndex)
	*/
	Signal2<UIList *, unsigned long> OnItemMouseOver, OnItemClick;

	UIList(UIManager *Manager) : UIPanel(Manager), FontSize(16)
	{
		OnConstructed();
		OnClick.Connect(this, &UIList::OnItemClickCheck);
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI Check Box Element
*/
class UICheckBox : public UIPanel
{
private:
	SuperSmartPointer<sf::Texture> CheckTexture, UnCheckTexture;
	unsigned long FontSize;
	Vector4 FontColor;
	bool Checked;
	Vector2 LabelOffset;

	void CheckMouseClick(UIPanel *Self);
	void OnSkinChange();
public:
	/*!
	*	The Checkbox's Caption Text
	*/
	std::string Caption;
	/*!
	*	Events for when the Checkbox is Checked or Unchecked
	*/
	Signal1<UICheckBox *> OnChecked, OnUnChecked;

	UICheckBox(UIManager *Manager) : UIPanel(Manager), Checked(false)
	{
		OnConstructed();
		OnClick.Connect(this, &UICheckBox::CheckMouseClick);
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);

	/*!
	*	\return Whether the Checkbox has been checked
	*/
	bool IsChecked()
	{
		return Checked;
	};

	/*!
	*	Set whether the Checkbox is checked
	*	\param Value whether the Checkbox is checked
	*/
	void SetChecked(bool Value)
	{
		Checked = Value;

		if(Checked)
		{
			OnChecked(this);
		}
		else
		{
			OnUnChecked(this);
		};
	};
};

/*!
*	UI Menu Element
*/
class UIMenu : public UIPanel
{
	friend class UIManager;
private:
	SuperSmartPointer<sf::Texture> BackgroundTexture, SelectorBackgroundTexture;
	Vector2 Padding, TextOffset;
	unsigned long SelectorPadding, ItemHeight;
	Rect TextureRect;
	unsigned long FontSize;
	Vector4 FontColor;

public:
	/*!
	*	UI Menu Item
	*/
	class Item
	{
	public:
		/*!
		*	The Item's Caption
		*/
		sf::String Caption;
		/*!
		*	The Item's User Data
		*/
		void *UserData;
		/*!
		*	The Item's Index
		*/
		unsigned long Index;
	};

private:
	std::vector<Item> Items;

	void OnSkinChange();
	void OnItemClick(UIPanel *Self);
	void CloseSelf(UIPanel *Self);

	UIMenu(UIManager *Manager) : UIPanel(Manager)
	{
		OnConstructed();
		OnClick.Connect(this, &UIMenu::OnItemClick);
		OnLoseFocus.Connect(this, &UIMenu::CloseSelf);
	};
public:

	/*!
	*	Adds an Item
	*	\param Caption the Item's Caption
	*	\param UserData the Item's User Data (Optional)
	*/
	void AddItem(const sf::String &Caption, void *UserData = NULL)
	{
		Item Out;
		Out.Caption = Caption;
		Out.UserData = UserData;
		Out.Index = Items.size();

		Items.push_back(Out);
	};

	/*!
	*	On Item Selected Event
	*/
	Signal1<Item *> OnItemSelected;

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI Dropdown Element
*/
class UIDropdown : public UIPanel
{
private:
	SuperSmartPointer<sf::Texture> BackgroundTexture, DropdownTexture;
	Vector2 TextOffset, DropdownOffset;
	float DropdownHeight;
	Rect TextureRect;

	void OnSkinChange();
	void OnItemClickCheck(UIPanel *Self);
	void SetSelectedItem(UIMenu::Item *Item);
public:
	/*!
	*	Selected Index for this Dropdown
	*/
	long SelectedIndex;

	/*!
	*	All items contained by this Dropdown
	*/
	std::vector<std::string> Items;
	/*!
	*	The Dropdown's Font Size
	*/
	unsigned long FontSize;

	/*!
	*	(UIDropdown Self)
	*/
	Signal1<UIDropdown *> OnItemClick;

	UIDropdown(UIManager *Manager) : UIPanel(Manager), FontSize(16), SelectedIndex(-1)
	{
		OnConstructed();
		OnClick.Connect(this, &UIDropdown::OnItemClickCheck);
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI Menu Bar Element
*/
class UIMenuBar : public UIPanel
{
	friend class UIManager;
public:
	/*!
	*	UI Menu Bar Item
	*/
	class Item
	{
	public:
		/*!
		*	The Item's Caption Text
		*/
		sf::String Caption;
		/*!
		*	The Item's User Data
		*/
		void *UserData;

		/*!
		*	The Item's Sub Items
		*/
		std::vector<sf::String> SubItems;
	};
private:
	std::vector<Item> Items;
	SuperSmartPointer<sf::Texture> SelectorBackgroundTexture;
	Vector2 Padding, TextOffset;
	unsigned long SelectorPadding;
	Rect TextureRect;
	unsigned long FontSize;
	Vector4 FontColor;

	void OnSkinChange();
	void OnItemClick(UIPanel *Self);
	void OnItemSelected(UIMenu::Item *Item);

	UIMenuBar(UIManager *Manager) : UIPanel(Manager)
	{
		OnConstructed();
		OnClick.Connect(this, &UIMenuBar::OnItemClick);
		OnMouseMove.Connect(this, &UIMenuBar::OnItemClick);
	};
public:
	/*!
	*	On Menu Item Selected Event
	*/
	Signal1<const sf::String &> OnMenuItemSelected;

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);

	/*!
	*	Adds an Item to the item list
	*	\param TheItem the Item to add
	*/
	void AddItem(const Item &TheItem)
	{
		Items.push_back(TheItem);
	};
};

/*!
*	UI Sprite Element
*	\note Will AutoSize on both sides
*	\note Will AutoSize to (0, 0) should you not set a texture
*/
class UISprite : public UIPanel
{
public:
	/*!
	*	The Element's Sprite
	*/
	Sprite TheSprite;

	UISprite(UIManager *Manager) : UIPanel(Manager)
	{
		OnConstructed();
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI Scroll Bar Element
*/
class UIScrollbar : public UIPanel
{
private:
	Rect BackgroundTextureRect, HandleTextureRect;
	unsigned long Padding;
	unsigned long MinSize;
	SuperSmartPointer<sf::Texture> BackgroundTexture, HandleTexture;
	bool Vertical;

	void OnSkinChange();
public:
	/*!
	*	Minimum Value
	*/
	unsigned long MinValue;
	/*!
	*	Maximum Value
	*/
	unsigned long MaxValue;
	/*!
	*	Current Step
	*/
	unsigned long CurrentStep;
	/*!
	*	How much to increase per step
	*/
	unsigned long ValueStep;

	UIScrollbar(UIManager *Manager, bool IsVertical) : UIPanel(Manager), MinValue(0), MaxValue(100), CurrentStep(0), ValueStep(1), Vertical(IsVertical)
	{
		OnConstructed();
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);

	/*!
	*	\return the current Value
	*/
	unsigned long Value()
	{
		if(CurrentStep > MaxValue / ValueStep)
			CurrentStep = MaxValue / ValueStep;

		return MinValue + CurrentStep * ValueStep;
	};

	/*!
	*	On Change Event
	*/
	Signal1<UIPanel *> OnChange;
};

/*!
*	UI Tooltip Element
*/
class UITooltip : public UIPanel
{
	friend class UIManager;
private:
	//Source Panel that triggered this Tooltip
	SuperSmartPointer<UIPanel> Source;

	UITooltip(UIManager *Manager) : UIPanel(Manager)
	{
		OnConstructed();
		FontSize = 16;
	};
public:
	/*!
	*	Override Text
	*	\note Should have any content to override the Tooltip text
	*/
	sf::String OverrideText;
	/*!
	*	Font Size
	*/
	unsigned long FontSize;

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI Window Element
*/
class UIWindow : public UIPanel
{
protected:
	SuperSmartPointer<sf::Texture> BackgroundTexture, CloseButtonTexture;
	Rect TextureRect;
	unsigned long Padding;
	bool Closed, Dragging;
	Vector4 TitleFontColor;
	unsigned long TitleFontSize, TitleHeight, TitleOffset;
	Vector2 TitlePosition, CloseButtonPosition, LastMousePosition;

	void OnSkinChange();
	void CheckClosePressed(UIPanel *This);
public:
	/*!
	*	The Window's Title
	*/
	std::string Title;

	UIWindow(UIManager *Manager) : UIPanel(Manager), Closed(false), Dragging(false)
	{
		OnConstructed();
		OnClick.Connect(this, &UIWindow::CheckClosePressed);
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);

	/*!
	*	\return Whether the Window is closed
	*/
	bool IsClosed();

	/*!
	*	Sets whether the Window is closed
	*	\param Value whether the Window is closed
	*/
	void SetClosed(bool Value);
	
	/*!
	*	\return the Window Title's Offset
	*/
	unsigned long GetTitleOffset()
	{
		return TitleOffset;
	};

	/*!
	*	\return the Window Title Height
	*/
	unsigned long GetTitleHeight()
	{
		return TitleHeight;
	};
};

/*!
*	UI Text Element Alignment
*/
namespace UITextAlignment
{
	enum
	{
		Left = FLAGVALUE(0), //!<Align Left
		Right = FLAGVALUE(1), //!<Align Right
		Center = FLAGVALUE(2), //!<Align Center
		VCenter = FLAGVALUE(3) //!<Align Vertical Center
	};
};

/*!
*	UI Text Element
*	\note Will not autosize
*/
class UIText : public UIPanel
{
	std::vector<sf::String> Strings;
	sf::String Text;

protected:
	void OnSkinChange();
public:
	/*!
	*	Font Size
	*/
	unsigned long FontSize;

	/*!
	*	Text Formatting Params
	*	\note Only colors and border are used, Position is ignored
	*/
	TextParams Params;

	/*!
	*	Text Alignment
	*/
	unsigned long TextAlignment;

	UIText(UIManager *Manager) : UIPanel(Manager), TextAlignment(UITextAlignment::Left)
	{
		OnConstructed();
	};

	/*!
	*	Sets the Text of this UIText
	*	\param String the new Text
	*	\param AutoExpandHeight whether the Height of this Element should be expanded for the text
	*	\note Required set so we recalculate the strings
	*/
	void SetText(const sf::String &String, bool AutoExpandHeight = false);
	/*!
	*	\return the Text of this UIText
	*/
	const sf::String &GetText();
	/*!
	*	\return the Text's size
	*/
	Vector2 GetTextSize();

	/*!
	*	\return the formatted text strings
	*/
	const std::vector<sf::String> GetStrings()
	{
		return Strings;
	};

	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI Message Box Element
*/
class UIMessageBox : public UIPanel
{
private:
	void OnMainButtonClicked(UIPanel *Self);
	void OnSecondaryButtonClicked(UIPanel *Self);
	void InitMessageBox();
	sf::String Title, Message, ButtonTitle, SecondaryButtonTitle;
	SuperSmartPointer<UIWindow> TheWindow;
	SuperSmartPointer<UIText> TheText;
public:
	/*!
	*	On Button Pressed Element
	*	Parameters: This MessageBox, the Button Index (0: Main Button, 1: Other Button)
	*/
	Signal2<UIMessageBox *, unsigned long> OnButtonPressed;

	UIMessageBox(UIManager *Manager, const sf::String &TitleStr, const sf::String &MessageStr, const sf::String &ButtonTitleStr, const sf::String &SecondaryButtonTitleStr = "") :
		UIPanel(Manager), Title(TitleStr), Message(MessageStr), ButtonTitle(ButtonTitleStr.getSize() ? ButtonTitleStr : "OK"),
			SecondaryButtonTitle(SecondaryButtonTitleStr.getSize() ? SecondaryButtonTitleStr : "")
	{
		OnConstructed();
		BlockingInput = true;

		InitMessageBox();
	};

	void PerformLayout();
	void Update(const Vector2 &ParentPosition);
	void Draw(const Vector2 &ParentPosition, sf::RenderWindow *Renderer);
};

/*!
*	UI Draggable Element
*	Provides Click+Drag behaviour
*/
class UIDraggable : public UIPanel
{
protected:
	bool Dragging;

	void OnMouseJustPressedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o);
	void OnMouseReleasedDraggable(UIPanel *This, const InputCenter::MouseButtonInfo &o);
public:
	/*!
	*	Drag Events for dragging, begin, and end
	*/
	Signal1<UIPanel *> OnDragBegin, OnDragging, OnDragEnd;

	UIDraggable(UIManager *Manager) : UIPanel(Manager), Dragging(false)
	{
		OnConstructed();
		OnMouseJustPressed.Connect(this, &UIDraggable::OnMouseJustPressedDraggable);
		OnMouseReleased.Connect(this, &UIDraggable::OnMouseReleasedDraggable);
	};

	/*!
	*	Draws the Draggable at a certain position
	*	\param MousePosition the Mouse's current Position
	*	\param Renderer the Renderer to use
	*/
	virtual void DrawDraggable(const Vector2 &MousePosition, sf::RenderWindow *Renderer) = 0;
};

/*!
*	UI Droppable Element
*	Provides Drop behaviour
*/
class UIDroppable : public UIPanel
{
public:
	/*!
	*	On Drop Event
	*/
	Signal1<UIPanel *> OnDrop;

	UIDroppable(UIManager *Manager) : UIPanel(Manager)
	{
		OnConstructed();
	};
};

/*!
*	UI Layout
*	Contains Elements and can be loaded from a scheme file
*/
class UILayout
{
friend class UIManager;
	UIManager *Owner;
public:
	typedef std::map<StringID, SuperSmartPointer<UIPanel> > ElementMap;
	ElementMap Elements;

	std::string Name;

	Json::Value ContainedObjects;

	~UILayout();

	SuperSmartPointer<UILayout> Clone(SuperSmartPointer<UIPanel> Parent, const std::string &ParentElementName);

	SuperSmartPointer<UIPanel> FindPanelById(StringID ID);
	SuperSmartPointer<UIPanel> FindPanelByName(const std::string &Name);
};

/*!
*	UI Manager
*	Handles all input and processing and rendering of UI Elements
*/
class UIManager
{
	friend class UIPanel;
	friend class UIMenu;
	friend class UILayout;
private:

	class ElementInfo
	{
	public:
		SuperSmartPointer<UIPanel> Panel;
		unsigned long DrawOrder;

		~ElementInfo()
		{
			Panel.Dispose();
		};
	};

	unsigned long DrawOrderCounter;

	typedef std::map<StringID, SuperSmartPointer<ElementInfo> > ElementMap;
	ElementMap Elements;

	SuperSmartPointer<UIPanel> FocusedElementValue;
	SuperSmartPointer<UITooltip> Tooltip;
	SuperSmartPointer<GenericConfig> Skin;
	SuperSmartPointer<sf::Font> DefaultFont;
	SuperSmartPointer<UIMenu> CurrentMenu;
	SuperSmartPointer<UIMenuBar> CurrentMenuBar;

	Vector4 DefaultFontColor, DefaultSecondaryFontColor;
	unsigned long DefaultFontSize;

	void OnMouseJustPressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMousePressedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMouseReleasedPriv(const InputCenter::MouseButtonInfo &o);
	void OnMouseMovePriv();
	void OnKeyPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyJustPressedPriv(const InputCenter::KeyInfo &o);
	void OnKeyReleasedPriv(const InputCenter::KeyInfo &o);
	void OnCharacterEnteredPriv();

	void RegisterInput();
	void UnRegisterInput();

	void RecursiveFindFocusedElement(const Vector2 &ParentPosition, UIPanel *p, UIPanel *&FoundElement);

	void RemoveMenuFuture(std::vector<unsigned char> &Stream);

	sf::RenderWindow *Owner;
	InputCenter *Input;

	typedef std::map<StringID, SuperSmartPointer<UILayout> > LayoutMap;
	LayoutMap Layouts;

	bool DrawOrderCacheDirty;
	std::vector<ElementInfo *> DrawOrderCache;

	void CopyElementsToLayout(SuperSmartPointer<UILayout> TheLayout, Json::Value &Elements, UIPanel *Parent, const std::string &ParentElementName);
public:

	UIManager(sf::RenderWindow *TheOwner, InputCenter *TheInput) : Owner(TheOwner), Input(TheInput), DrawOrderCounter(0)
	{
		Tooltip.Reset(new UITooltip(this));
		RegisterInput();
	};

	~UIManager() { UnRegisterInput(); Clear(); };

	/*!
	*	\return the Renderer that owns this UI Manager
	*/
	sf::RenderWindow *GetOwner()
	{
		return Owner;
	};

	/*!
	*	\return the Input that this UI Manager uses
	*/
	InputCenter *GetInput()
	{
		return Input;
	};

	/*!
	*	Updates all visible Elements
	*/
	void Update();

	/*!
	*	Draws all visible Elements
	*	\param Renderer the Renderer to draw to
	*/
	void Draw(sf::RenderWindow *Renderer);

	/*!
	*	Adds an UI element to this UI Manager
	*	\param ID the UI Element's ID
	*	\param Element the UI Element to add
	*	\note You must be careful not to have multiple Elements with the same ID
	*	\return whether the element was added (fails if there was something wrong like a name conflict)
	*/
	bool AddElement(StringID ID, SuperSmartPointer<UIPanel> Element);

	/*!
	*	Removes an Element
	*	\param ID the UI Element's ID
	*/
	void RemoveElement(StringID ID);

	/*!
	*	Loads one or more Layouts from a Stream
	*	Format is JSON:
	*	[
	*		"GUILayoutSample" : [
	*			"Window": {
	*				"Control": "Frame",
	*				"Xpos": "2%", //May be absolute pixels too
	*				"Ypos": "2%",
	*				"Wide": "96%",
	*				"Tall": "96%",
	*	
	*				"Children": [
	*					"OK": {
	*						"Control": "Button",
	*						"Xpos": "center",
	*						"Ypos": "center",
	*						"Caption": "Test"
	*					}
	*				]
	*			}
	*		]
	*	]
	*
	*	\param In the stream
	*	\param Parent the parent element if there is one
	*/
	bool LoadLayouts(const std::string &Data, SuperSmartPointer<UIPanel> Parent = SuperSmartPointer<UIPanel>());

	/*!
	*	Clears all layouts we have loaded
	*	Does not remove regular elements!
	*/
	void ClearLayouts();

	/*!
	*	Fetches an Element by ID
	*	\param ID the Element's ID
	*	\return an Element or Empty
	*/
	SuperSmartPointer<UIPanel> GetElement(StringID ID);

	/*!
	*	\return the currently Focused Element
	*/
	SuperSmartPointer<UIPanel> GetFocusedElement();

	/*!
	*	\return the element that has the mouse moving over
	*/
	SuperSmartPointer<UIPanel> GetMouseOverElement();

	/*!
	*	\return the UI Tooltip
	*/
	UITooltip &GetTooltip();

	/*!
	*	\return the Default Font Color
	*/
	const Vector4 &GetDefaultFontColor()
	{
		return DefaultFontColor;
	};

	/*!
	*	\return the Default Font Color
	*/
	const Vector4 &GetDefaultSecondaryFontColor()
	{
		return DefaultSecondaryFontColor;
	};

	/*!
	*	\return the Default Font Size
	*/
	unsigned long GetDefaultFontSize()
	{
		return DefaultFontSize;
	};

	/*!
	*	\return the Default Font
	*/
	SuperSmartPointer<sf::Font> GetDefaultFont()
	{
		return DefaultFont;
	};

	/*!
	*	Creates an UI Menu at a certain position
	*	\param Position the UI Menu's Position
	*	\return the UI Menu that was created
	*/
	UIMenu *CreateMenu(const Vector2 &Position);

	/*!
	*	\return the current UI Menu
	*/
	UIMenu *GetCurrentMenu()
	{
		return CurrentMenu.Get();
	};

	/*!
	*	\return the UI Menu Bar that was created
	*/
	UIMenuBar *CreateMenuBar();

	/*!
	*	\return the Current UI Menu Bar
	*/
	UIMenuBar *GetCurrentMenuBar()
	{
		return CurrentMenuBar.Get();
	};

	/*!
	*	Removes all elements
	*/
	void Clear();

	/*!
	*	Sets the UI Manager's UI Skin
	*	\param Skin the new Skin
	*/
	void SetSkin(SuperSmartPointer<GenericConfig> Skin);

	/*!
	*	\return the UI Skin we are currently using
	*/
	SuperSmartPointer<GenericConfig> GetSkin()
	{
		return Skin;
	};

	/*!
	*	Removes Focus from the focused UI Element
	*/
	void ClearFocus()
	{
		FocusedElementValue = SuperSmartPointer<UIPanel>();
	};
};
