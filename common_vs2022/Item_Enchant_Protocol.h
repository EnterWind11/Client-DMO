#pragma once
#pragma once

#include "common_vs2022/pItem.h"
#include "common_vs2022/Protocol/ItemScan_Define.h"



class GS2C_RECV_ENCHANT_ITEM_ENCHANT
{
public:
	GS2C_RECV_ENCHANT_ITEM_ENCHANT() : nProtocol(pItem::AccStoneUse), nInvenPos(0), nRate(0), nLevel(0), nAccOption(), nAccValues()
	{
	}
	WORD GetProtocol() { return nProtocol; }
public:
	WORD nProtocol;
	u1 nResult;
	u2 nInvenPos;
	u1 nRate;
	u1 nLevel;
	u2 nAccOption[nLimit::MAX_ACCESSORY_OPTION];
	u2 nAccValues[nLimit::MAX_ACCESSORY_OPTION];



};