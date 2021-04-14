#include "MsgDebugString.h"

MsgDebugString::MsgDebugString(const std::string& string)
{
	Header = { sizeof(MsgHeader) + static_cast<int>(string.length()) + 1, MsgType::MsgDebugString };
	memset(Body, 0, string.length() + 1);
	memcpy(Body, string.c_str(), string.length());
}


std::string MsgDebugString::GetDebugString()
{
	if (!Body)
		return std::string();
	return std::string(reinterpret_cast<char*>(Body));
}
