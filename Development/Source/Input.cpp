#include <vector>
#include <map>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <json/json.h>
#include "Math.hpp"
#include "SuperSmartPointer.hpp"
#include "StringID.hpp"
#include "Signal.h"
#include "Delegate.h"
using namespace Gallant;
#include "Input.hpp"

InputCenter::InputCenter() : HasFocus(true)
{
	for(wchar_t i = 0; i < sf::Keyboard::KeyCount; i++)
	{
		Keys[i].Name = i;
	};

	for(unsigned char i = 0; i < sf::Mouse::ButtonCount; i++)
	{
		MouseButtons[i].Name = i;
	};

	MouseWheel = 0;
};

bool InputCenter::Update(sf::RenderWindow *Renderer)
{
	if(Renderer == NULL)
		return false;

	for(unsigned long i = 0; i < sf::Keyboard::KeyCount; i++)
	{
		Keys[i].JustPressed = false;
	};

	for(unsigned long i = 0; i < sf::Mouse::ButtonCount; i++)
	{
		MouseButtons[i].JustPressed = MouseButtons[i].JustReleased = false;
	};

	Character = L'\0';

	MouseMovement = Vector3(MousePosition.x, MousePosition.y, (float)MouseWheel);

	MouseWheel = 0;

	static sf::Event Event;

	bool HasPendingResize = false;
	unsigned long PendingResizeWidth, PendingResizeHeight;

	while(Renderer->pollEvent(Event))
	{
		switch(Event.type)
		{
		case sf::Event::MouseLeft:
			for(unsigned long i = 0; i < sf::Mouse::ButtonCount; i++)
			{
				MouseButtons[i].Pressed = MouseButtons[i].JustPressed = false;
				MouseButtons[i].OnReleased(MouseButtons[i]);
			};

			break;
		case sf::Event::MouseEntered:
			for(unsigned long i = 0; i < sf::Mouse::ButtonCount; i++)
			{
				MouseButtons[i].Pressed = MouseButtons[i].JustPressed = sf::Mouse::isButtonPressed((sf::Mouse::Button)i);

				if(MouseButtons[i].JustPressed)
				{
					MouseButtons[i].OnJustPressed(MouseButtons[i]);
				};
			};

			break;
		case sf::Event::KeyPressed:
			{
				if(Event.key.code == -1)
					break;

				KeyInfo &Key = Keys[Event.key.code];
				Key.Control = Event.key.control;
				Key.Alt = Event.key.alt;
				Key.Shift = Event.key.shift;
				Key.Pressed = Key.JustPressed = true;

				Key.OnJustPressed(Key);
			};

			break;
		case sf::Event::KeyReleased:
			{
				if(Event.key.code == -1)
					break;

				KeyInfo &Key = Keys[Event.key.code];
				Key.Control = Event.key.control;
				Key.Alt = Event.key.alt;
				Key.Shift = Event.key.shift;
				Key.Pressed = Key.JustPressed = false;

				Key.OnReleased(Key);
			};

			break;
		case sf::Event::MouseMoved:
			MousePosition.x = (float)Event.mouseMove.x;
			MousePosition.y = (float)Event.mouseMove.y;

			break;
		case sf::Event::MouseWheelMoved:
			MouseWheel = Event.mouseWheel.delta;

			break;
		case sf::Event::MouseButtonPressed:
			{
				MouseButtonInfo &Button = MouseButtons[Event.mouseButton.button];
				Button.Pressed = Button.JustPressed = true;
			};

			break;
		case sf::Event::MouseButtonReleased:
			{
				MouseButtonInfo &Button = MouseButtons[Event.mouseButton.button];
				Button.Pressed = Button.JustPressed = false;
				Button.JustReleased = true;
			};

			break;
		case sf::Event::Closed:
			return false;

			break;
		case sf::Event::TextEntered:
			Character = Event.text.unicode;

			OnCharacterEntered();

			break;
		case sf::Event::Resized:
			HasPendingResize = true;
			PendingResizeWidth = Event.size.width;
			PendingResizeHeight = Event.size.height;

			break;
		case sf::Event::GainedFocus:
			HasFocus = true;

			OnGainFocus();

			break;
		case sf::Event::LostFocus:
			HasFocus = false;

			OnLoseFocus();

			break;
		};
	};

	if(HasPendingResize)
	{
		//Renderer->HandleResize(PendingResizeWidth, PendingResizeHeight);
	};

	Vector3 NewFrameMouseMovement(MousePosition.x, MousePosition.y, (float)MouseWheel);
	MouseMovement = NewFrameMouseMovement - MouseMovement;

	if(MouseMovement != Vector3(0, 0, 0))
		OnMouseMove();

	for(unsigned long i = 0; i < sf::Keyboard::KeyCount; i++)
	{
		if(Keys[i].Pressed && !Keys[i].JustPressed)
		{
			Keys[i].OnPressed(Keys[i]);
		};
	};

	for(unsigned long i = 0; i < sf::Mouse::ButtonCount; i++)
	{
		if(MouseButtons[i].Pressed)
		{
			if(MouseButtons[i].JustPressed)
			{
				MouseButtons[i].OnJustPressed(MouseButtons[i]);
			}
			else
			{
				MouseButtons[i].OnPressed(MouseButtons[i]);
			};
		}
		else if(MouseButtons[i].JustReleased)
		{
			MouseButtons[i].OnReleased(MouseButtons[i]);
		};
	};

	return true;
};

void InputCenter::CenterMouse(sf::RenderWindow *Renderer)
{
	sf::Vector2i HalfMousePosition((unsigned long)Renderer->getSize().x / 2, (unsigned long)Renderer->getSize().y / 2);

	sf::Mouse::setPosition(HalfMousePosition, *Renderer);

	MousePosition = Vector2((float)HalfMousePosition.x, (float)HalfMousePosition.y);
};
