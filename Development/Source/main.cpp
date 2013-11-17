#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <json/json.h>
#include <sstream>
#include <math.h>
#include <stdio.h>
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

int main(int argc, char **argv)
{
	sf::RenderWindow Window;
	Window.create(sf::VideoMode(960, 600), "GUISandbox");

	if(!Window.isOpen())
		return 1;

	InputCenter Input;
	UIManager UI(&Window, &Input);

	FILE *In = fopen("UIThemes/PolyCode/skin.cfg", "rt");

	if(!In)
		return false;

	fseek(In, 0, SEEK_END);

	int Size = ftell(In);

	fseek(In, 0, SEEK_SET);

	std::string Skin;

	Skin.resize(Size);

	fread(&Skin[0], sizeof(char), Size, In);

	fclose(In);

	Skin.resize(strlen(Skin.c_str()));

	SuperSmartPointer<GenericConfig> Config(new GenericConfig());

	if(!Config->DeSerialize(Skin))
		return false;

	UI.SetSkin(Config);

	In = fopen("GUILayout.resource", "rt");

	if(!In)
		return false;

	fseek(In, 0, SEEK_END);

	Size = ftell(In);

	fseek(In, 0, SEEK_SET);

	std::string Layout;

	Layout.resize(Size);

	fread(&Layout[0], sizeof(char), Size, In);

	fclose(In);

	Layout.resize(strlen(Layout.c_str()));
	
	if(!UI.LoadLayouts(Layout))
		return false;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, Window.getSize().x, Window.getSize().y, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);

	while(Input.Update(&Window))
	{
		UI.Update();
		Future::Instance.Update();

		Window.clear(sf::Color(255, 255, 255, 255));

		UI.Draw(&Window);

		Window.display();
	};

	return 0;
};
