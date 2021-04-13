#pragma once
#include "Msg.h"
#include <string>

class MsgDebugString : public Msg
{
public:
	MsgDebugString() = default;
	MsgDebugString(const std::string& string);

public:
	std::string GetDebugString();
};

typedef std::shared_ptr<MsgDebugString> MsgDebugStringPtr;