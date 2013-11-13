#pragma once
/*!
	CRC32 Helper Class
*/
class CRC32
{
private:
	unsigned long LookupTable[256];
	unsigned long Reflect(unsigned long Reflect, char Char);
public:
	static CRC32 Instance;

	CRC32();
	
	/*!
		Calculates the CRC of a piece of data
		\param Data the Data to calculate
		\param DataLength the Length in bytes of the Data
		\return the CRC as a unsigned long
	*/
	unsigned long CRC(const unsigned char *Data, unsigned long DataLength);
};
