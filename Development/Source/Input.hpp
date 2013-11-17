#pragma once
class InputCenter
{
public:
	struct KeyInfo
	{
		wchar_t Name;
		bool Alt, Control, Shift;
		bool Pressed, JustPressed;

		KeyInfo() : Name(0), Alt(false), Control(false), Shift(false),
			Pressed(false), JustPressed(false) {};

		Signal1<const KeyInfo &> OnPressed, OnJustPressed, OnReleased; //Params: Self
	};

	struct MouseButtonInfo
	{
		unsigned char Name;
		bool Pressed, JustPressed, JustReleased;

		MouseButtonInfo() : Pressed(false), JustPressed(false), JustReleased(false) {};

		Signal1<const MouseButtonInfo &> OnPressed, OnJustPressed, OnReleased; //Params: Self
	};

	KeyInfo Keys[sf::Keyboard::KeyCount];
	MouseButtonInfo MouseButtons[sf::Mouse::ButtonCount];

	Signal0<> OnMouseMove, OnCharacterEntered, OnGainFocus, OnLoseFocus;

	bool HasFocus;
	Vector2 MousePosition;
	wchar_t Character;
	long MouseWheel;
	Vector3 MouseMovement;

	InputCenter();
	bool Update(sf::RenderWindow *Renderer);
	void CenterMouse(sf::RenderWindow *Renderer);

	/*!
	*	\return Whether the current input event was consumed by something else
	*/
	bool InputConsumed();

	/*!
	*	Consumes input for this event
	*/
	void ConsumeInput();

private:
	bool InputConsumedValue;
};
