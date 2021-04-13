#include "MsgBuilder.h"
#include "MsgDebugString.h"


MsgPtr MsgBuilder::Build(MsgHeader header, unsigned char* body)
{
	MsgPtr msg;
	switch (header.Type)
	{
	case MsgType::MsgDebugString:
	{
		msg = std::make_shared<MsgDebugString>();
		break;
	}
	default:
		msg = std::make_shared<Msg>();
		break;
	}
	msg->Header = header;
	memcpy(msg->Body, body, header.Length);
    return msg;
}
