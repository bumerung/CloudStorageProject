#pragma once
#include "Msg.h"
#include <string>
class MsgDebugString : public Msg
{

public:
	std::string GetDebugString();
};

typedef std::shared_ptr<MsgDebugString> MsgDebugStringPtr;