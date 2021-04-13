#pragma once
class MsgCryptography
{
public:
	static void Encrypt(char* msg, unsigned int length);
	static void Decrypt(char* msg, unsigned int length);
};

