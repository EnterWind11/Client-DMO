#include "Channel_Define.h"

class GS2C_RECV_CURRENT_CHANNEL
{
public:
	GS2C_RECV_CURRENT_CHANNEL():channel_idx(0),m_nResult(0)
	{
	}

	int		m_nResult;

#ifdef COMPAT_487
	u4	channel_idx;
#else
	uint	channel_idx;
#endif
				// 접속중인 채널 번호
};

class GS2C_RECV_CHANNEL_LISTINFO
{
public:
	GS2C_RECV_CHANNEL_LISTINFO():m_nResult(0)
	{
		memset( channel, 0xFF, sizeof(channel) );
	}

	int		m_nResult;
	char	channel[nLimit::Channel];	// 현재 가동중인 채널 상태, -1:미사용, 0:원활, 1:많음, 2:너무많음, 3~10:혼잡
};