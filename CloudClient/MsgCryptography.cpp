#include "MsgCryptography.h"

void MsgCryptography::Encrypt(char* msg, unsigned int length)
{
	for (size_t i = 0; i < length; i++)
	{
		msg[i] ^= 0xAB;
	}
}

void MsgCryptography::Decrypt(char* msg, unsigned int length)
{
	for (size_t i = 0; i < length; i++)
	{
		msg[i] ^= 0xAB;
	}
}
