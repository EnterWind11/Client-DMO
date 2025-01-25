#pragma once

#include "common_vs2022/pItem.h"
#include "common_vs2022/Protocol/ItemScan_Define.h"



class GS2C_RECV_ITEMIDENTIFY
{
public:
	GS2C_RECV_ITEMIDENTIFY() : nProtocol(pItem::AccessoryCheck), nInvenPos(0), nRate(0),nLevel(0), nAccOption(), nAccValues()
	{
	}
	WORD GetProtocol() { return nProtocol; }
public:
	WORD nProtocol;
	u2 nInvenPos;
	u1 nRate;
	u1 nLevel;
	u2 nAccOption[nLimit::MAX_ACCESSORY_OPTION];
	u2 nAccValues[nLimit::MAX_ACCESSORY_OPTION];
	


};