#pragma once
namespace ValueTypes
{
	enum
	{
		Unknown = 0,
		Int32,
		Float,
	};
};

class GenericConfig
{
public:
	class Value
	{
	public:
		std::string Content;
		unsigned long Type;

		Value();
	};

	class IntValue : public Value
	{
	public:
		long Value;

		IntValue();
		IntValue(long Value);
		IntValue(const char *StringValue);
	};

	class FloatValue : public Value
	{
	public:
		float Value;

		FloatValue();
		FloatValue(float Value);
		FloatValue(const char *StringValue);
	};
private:
	class Section
	{
	public:
		typedef std::map<std::string, Value> ValueMap;
		ValueMap Values;
	};

	typedef std::map<std::string, Section> SectionMap;
	SectionMap Sections;
public:
	std::string Serialize();
	bool DeSerialize(const std::string &Data);

	Value GetValue(const char *SectionName, const char *ValueName, const Value &Default);
	long GetInt(const char *SectionName, const char *ValueName, long Default);
	float GetFloat(const char *SectionName, const char *ValueName, float Default);
	std::string GetString(const char *SectionName, const char *ValueName, const std::string &Default = "");
	void SetValue(const char *SectionName, const char *ValueName, const char *Content);
};
