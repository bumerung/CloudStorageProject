#pragma once
#include "Msg.h"


class MsgBuilder
{
public:
	static MsgPtr Build(MsgHeader header, unsigned char* body);
};

