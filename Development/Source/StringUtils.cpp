#include <stdio.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include "StringUtils.hpp"

std::vector<std::string> StringUtils::Split(const std::string &String, char Separator)
{
	std::vector<std::string> Fragments;
	long Offset = 0;
	long Index = String.find(Separator);

	while(Index != std::string::npos)
	{
		Fragments.push_back(String.substr(Offset, Index - Offset));
		Offset = Index + 1;

		Index = String.find(Separator, Offset);
	};

	if(String.length() > (unsigned long)Offset)
		Fragments.push_back(String.substr(Offset));

	return Fragments;
};

std::string StringUtils::Strip(const std::string &str, char c)
{
	std::vector<char> Out;

	for(unsigned long i = 0; i < str.length(); i++)
	{
		if(str[i] == c)
			continue;

		Out.push_back(str[i]);
	};

	Out.push_back(0);

	return &Out[0];
};

std::string StringUtils::Join(const std::string &Separator, const std::vector<std::string> &Fragments)
{
	if(Fragments.size() == 0)
		return "";

	static std::stringstream str;
	str.str(Fragments[0]);

	for(unsigned long i = 1; i < Fragments.size(); i++)
	{
		str << Separator << Fragments[i];
	};

	return str.str();
};

unsigned long StringUtils::MakeIntFromChars(char a, char b, char c, char d)
{
	unsigned long Out;

	char *Temp = (char *)&Out;

	Temp[0] = a;
	Temp[1] = b;
	Temp[2] = c;
	Temp[3] = d;

	return Out;
};

unsigned long StringUtils::HexToInt(const std::string &str)
{
	unsigned long Temp = 0;

	sscanf(str.c_str(), "%x", &Temp);

	return Temp;
};

float StringUtils::HexToFloat(const std::string &str)
{
	unsigned long Temp = 0;

	sscanf(str.c_str(), "%x", &Temp);

	return Temp / 255.f;
};

std::string StringUtils::MakeIntString(const long &i, bool Hex)
{
	std::stringstream str;

	if(Hex)
		str << std::hex;

	str << i;

	return str.str();
};

std::string StringUtils::MakeFloatString(const float &f)
{
	std::stringstream str;
	str << f;

	return str.str();
};

std::string StringUtils::MakeByteString(const unsigned char &u)
{
	std::stringstream str;
	str << (unsigned long)u;

	return str.str();
};

std::string StringUtils::ToUpperCase(const std::string &str)
{
	std::string Out = str;

	std::transform(str.begin(), str.end(), Out.begin(), toupper);

	return Out;
};

std::string StringUtils::ToLowerCase(const std::string &str)
{
	std::string Out = str;

	std::transform(str.begin(), str.end(), Out.begin(), tolower);

	return Out;
};
