#pragma once

class CProtoBufMsgBase;
class CMsgClientWalletInfoUpdate;
class CMsgClientEmailAddrInfo;

namespace Misc
{
	bool shouldFakeOffline();
	void recvMsg(CProtoBufMsgBase* msg);

	bool recvWalletInfo(CMsgClientWalletInfoUpdate* body);
	bool recvEmailInfo(CMsgClientEmailAddrInfo* body);
}
