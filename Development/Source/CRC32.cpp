#include "CRC32.hpp"

CRC32 CRC32::Instance;

CRC32::CRC32()
{
	unsigned long Poly = 0x04C11DB7;
	unsigned long CRC;

	for(unsigned long i = 0; i <= 0xFF; i++)
	{
		unsigned long &Value = LookupTable[i];

        CRC = i << 24;
        
        for(unsigned long j = 0; j < 8; j++)
        {
            CRC = (CRC & 0x80000000) != 0 ? (CRC << 1 ^ Poly) : (CRC << 1);
        };

		Value = CRC;
	};
};

unsigned long CRC32::CRC(const unsigned char *Data, unsigned long DataLength)
{
	unsigned long OutCRC = 0xFFFFFFFF;

	while(DataLength--)
		OutCRC = (OutCRC >> 8) ^ LookupTable[(OutCRC & 0xFF) ^ *Data++];

	return OutCRC ^ 0xFFFFFFFF;
};
