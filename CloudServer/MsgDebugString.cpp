#include "MsgDebugString.h"

std::string MsgDebugString::GetDebugString()
{
	if (!Body)
		return std::string();
	return std::string(reinterpret_cast<char*>(Body));
}
