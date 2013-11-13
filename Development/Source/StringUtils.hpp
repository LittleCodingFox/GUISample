#pragma once
/*!
	String Utilities
*/
class StringUtils
{
public:
	/*!
		Splits a String
		\param String the String to split
		\param Separator the Separator of the String
		\return the String Fragments
		\sa Join
	*/
	static std::vector<std::string> Split(const std::string &String, char Separator);
	/*!
		Joins a split String
		\param Separator the Separator to use when merging the Strings
		\param Fragments the String Fragments
		\return the merged string
		\sa Split
	*/
	static std::string Join(const std::string &Separator, const std::vector<std::string> &Fragments);

	/*!
		Makes an unsigned long from four characters
		\param a the first character
		\param b the second character
		\param c the third character
		\param d the last character
		\return a unsigned long containing the four characters
	*/
	static unsigned long MakeIntFromChars(char a, char b, char c, char d);

	/*!
		Converts a Hex string to a unsigned long
		\param str the String to convert
		\return the converted unsigned long
	*/
	static unsigned long HexToInt(const std::string &str);

	/*!
		Converts a Hex string to a float
		\param str the String to convert
		\return the converted float
	*/
	static float HexToFloat(const std::string &str);

	/*!
		Converts a long to a String
		\param i the long to convert
		\param Hex whether to convert to a Hex string
		\return the converted string
	*/
	static std::string MakeIntString(const long &i, bool Hex = false);

	/*!
		Converts a float to a String
		\param f the float to convert
		\return the converted string
	*/
	static std::string MakeFloatString(const float &f);

	/*!
		Converts a unsigned char to a String
		\param u the unsigned char to convert
		\return the converted string
	*/
	static std::string MakeByteString(const unsigned char &u);

	/*!
		Strips a String from a character
		\param str the string to strip
		\param c the character to remove
		\return the stripped string
	*/
	static std::string Strip(const std::string &str, char c);

	/*!
		Uppercases a string
		\param str the string to uppercase
		\return the uppercased string
	*/
	static std::string ToUpperCase(const std::string &str);

	/*!
		Lowercases a string
		\param str the string to Lowercase
		\return the lowercased string
	*/
	static std::string ToLowerCase(const std::string &str);
};
