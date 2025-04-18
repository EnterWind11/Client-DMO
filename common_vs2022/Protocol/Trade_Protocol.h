#pragma once

#include "Trade_Define.h"
#include "../cItemData.h"

//////////////////////////////////////////////////////////////////////////
//거래 요청 받음.
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_TRADE_REQUEST
{
public:
	GS2C_RECV_TRADE_REQUEST() : m_dwResult(0),m_TargetTamerUID(0)
	{}

public:	
	int						m_dwResult;
	u2						m_TargetTamerUID;
};

//////////////////////////////////////////////////////////////////////////
// 거래 요처에 대한 결과값
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_TRADE_REQUEST_RESULT
{
public:
	GS2C_RECV_TRADE_REQUEST_RESULT() : m_dwResult(0),m_TargetTamerUID(0)
	{}

public:	
	int						m_dwResult;
	u2						m_TargetTamerUID;
};

//////////////////////////////////////////////////////////////////////////
// 거래중 아이템 등록
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_TRADE_ITEM_REGIST
{
public:
	GS2C_RECV_TRADE_ITEM_REGIST() : m_dwResult(0),m_TargetTamerUID(0)
	{}

public:	
	int						m_dwResult;
	u2						m_TargetTamerUID;
	cItemData				m_ItemData;
	n1						m_TradeInvenSlotNum;
	u2						m_InventorySlotNum;
};

//////////////////////////////////////////////////////////////////////////
// 거래중 아이템 제거
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_TRADE_ITEM_UNREGIST
{
public:
	GS2C_RECV_TRADE_ITEM_UNREGIST() : m_dwResult(0),m_TargetTamerUID(0),m_nItemPosition(0)
	{}

public:	
	int						m_dwResult;
	u2						m_TargetTamerUID;
	n1						m_nItemPosition;
};

//////////////////////////////////////////////////////////////////////////
// 거래중 돈 변경
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_TRADE_MONEY_CHANGE
{
public:
	GS2C_RECV_TRADE_MONEY_CHANGE() : m_dwResult(0),m_TargetTamerUID(0),m_nMoney(0)
	{}

public:	
	int						m_dwResult;
	u2						m_TargetTamerUID;
	u4						m_nMoney;
};

//////////////////////////////////////////////////////////////////////////
// 거래 인벤토리 잠금/해제
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_TRADE_INVEN_LOCK
{
public:
	GS2C_RECV_TRADE_INVEN_LOCK() : m_dwResult(0),m_TargetTamerUID(0),m_bLock(false)
	{}

public:	
	int						m_dwResult;
	u2						m_TargetTamerUID;
	bool					m_bLock;
};

//////////////////////////////////////////////////////////////////////////
// 거래 완료 대지중
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_TRADE_STANDBY
{
public:
	GS2C_RECV_TRADE_STANDBY() : m_dwResult(0),m_TargetTamerUID(0)
	{}

public:	
	int						m_dwResult;
	u2						m_TargetTamerUID;
};

//////////////////////////////////////////////////////////////////////////
// 거래 최종 결과값
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_TRADE_FINAL_RESULT
{
public:
	GS2C_RECV_TRADE_FINAL_RESULT() : m_dwResult(0)
	{}

public:	
	int					m_dwResult;
};
