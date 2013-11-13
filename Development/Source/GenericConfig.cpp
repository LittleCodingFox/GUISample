#include <vector>
#include <map>
#include <sstream>
#include <stdio.h>
#include <SFML/System.hpp>
#include "SuperSmartPointer.hpp"
#include "GenericConfig.hpp"
#include "StringUtils.hpp"

GenericConfig::Value::Value() : Type(ValueTypes::Unknown) {};

GenericConfig::IntValue::IntValue() : Value()
{
	Type = ValueTypes::Int32;
	Value = 0xBAADF00D;
};

GenericConfig::IntValue::IntValue(long Value) : Value()
{
	Type = ValueTypes::Int32;
	this->Value = Value;
	Content.resize(256);
	sprintf(&Content[0], "%d", Value);
};

GenericConfig::IntValue::IntValue(const char *ValueString) : Value()
{
	Type = ValueTypes::Int32;

	if(sscanf(ValueString, "%d", &Value) <= 0)
	{
		Value = 0xBAADF00D;
		Content.resize(256);
		sprintf(&Content[0], "%d", Value);
	};
};

GenericConfig::FloatValue::FloatValue() : Value()
{
	Type = ValueTypes::Float;
	Value = (float)0xBAADF00D;
};

GenericConfig::FloatValue::FloatValue(float Value) : Value()
{
	Type = ValueTypes::Float;
	this->Value = Value;
	Content.resize(256);
	sprintf(&Content[0], "%f", Value);
};

GenericConfig::FloatValue::FloatValue(const char *ValueString) : Value()
{
	Type = ValueTypes::Float;

	if(sscanf(ValueString, "%f", &Value) <= 0)
	{
		Value = (float)0xBAADF00D;
		Content.resize(256);
		sprintf(&Content[0], "%f", Value);
	};
};

GenericConfig::Value GenericConfig::GetValue(const char *SectionName, const char *ValueName, const Value &Default)
{
	SectionMap::iterator it = Sections.find(SectionName);

	if(it == Sections.end())
		return Default;

	Section::ValueMap::iterator vit = it->second.Values.find(ValueName);

	if(vit == it->second.Values.end())
		return Default;

	return vit->second;
};

long GenericConfig::GetInt(const char *SectionName, const char *ValueName, long Default)
{
	SectionMap::iterator it = Sections.find(SectionName);

	if(it == Sections.end())
		return Default;

	Section::ValueMap::iterator vit = it->second.Values.find(ValueName);

	if(vit == it->second.Values.end())
		return Default;

	IntValue Out(vit->second.Content.c_str());

	return Out.Value != 0xBAADF00D ? Out.Value : Default;
};

float GenericConfig::GetFloat(const char *SectionName, const char *ValueName, float Default)
{
	SectionMap::iterator it = Sections.find(SectionName);

	if(it == Sections.end())
		return Default;

	Section::ValueMap::iterator vit = it->second.Values.find(ValueName);

	if(vit == it->second.Values.end())
		return Default;

	FloatValue Out(vit->second.Content.c_str());

	return Out.Value != 0xBAADF00D ? Out.Value : Default;
};

void GenericConfig::SetValue(const char *SectionName, const char *ValueName, const char *Content)
{
	Sections[SectionName].Values[ValueName].Content = Content;
};

std::string GenericConfig::Serialize()
{
	std::stringstream str;

	for(SectionMap::iterator it = Sections.begin(); it != Sections.end(); it++)
	{
		str << "[" << it->first << "]\n";

		for(Section::ValueMap::iterator vit = it->second.Values.begin(); vit != it->second.Values.end(); vit++)
		{
			str << vit->first << "=" << vit->second.Content << "\n";
		};
	};

	return str.str();
};

bool GenericConfig::DeSerialize(const std::string &Data)
{
	Sections.clear();

	std::string str = StringUtils::Strip(Data, '\r');
	std::vector<std::string> Lines = StringUtils::Split(str, '\n');

	std::string ActiveSection;

	for(unsigned long i = 0; i < Lines.size(); i++)
	{
		if(Lines[i].length() == 0 || Lines[i][0] == '#')
			continue;

		if(Lines[i][0] == '[')
		{
			if(Lines[i][Lines[i].length() - 1] != ']')
			{
				ActiveSection = std::string();

				continue;
			};

			ActiveSection = Lines[i].substr(1, Lines[i].length() - 2);
		}
		else if(ActiveSection.length())
		{
			long Index = Lines[i].find('=');

			if(Index == std::string::npos)
			{
				continue;
			};

			Sections[ActiveSection].Values[Lines[i].substr(0, Index)].Content = Lines[i].substr(Index + 1);
		};
	};

	return true;
};

std::string GenericConfig::GetString(const char *SectionName, const char *ValueName, const std::string &Default)
{
	Value DefaultValue;
	DefaultValue.Content = Default;

	return GetValue(SectionName, ValueName, DefaultValue).Content;
};
