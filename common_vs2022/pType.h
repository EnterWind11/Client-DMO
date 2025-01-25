

#pragma once


#include "nlib/base.h"
#include "nlib/cpacket.h"


namespace nLIB
{
	enum eType
	{
		SVR_NONE, SVR_GAME, SVR_BATTLE, SVR_DUNGEON,

	};
}


// class
namespace nClass
{
	enum
	{
		None,

		Digimon,
		Tamer,
		Item,
		Monster,
		Npc,

		Party,	// Classe excepcional, usada ao lidar com direitos de aquisição de itens
		CommissionShop,

		End
	}; // enum
}; // namespace nClass



// object type...
namespace nType
{
	enum
	{
		None,
	};

}; // namespace nType



__forceinline
uint GetClass(u2 nUID) { return nUID >> 14; }

__forceinline
uint GetIDX(u2 nUID) { return nUID & 0x0FFF; }


// base type data
class
cType	// Usado ao processar relacionado à sincronização de objeto de campo
{
public:
	cType(void);
	cType(uint uid);
	~cType(void);

protected:
	void SetClass(uint nClass);

public:
	u4 GetType(void) const;	// nType 참조
	u4 GetClass(void);	// nClass referência
	u2 GetUID(void);	// unique id	(nClass valor incluido)
	u2 GetIDX(void);	// nClass Dentro de unique idx
	void	Reset();
	void	SetTypeAll(u4 TypeAll = 0);

public:
	bool IsDigimon(void);
	bool IsTamer(void);
	bool IsItem(void);
	bool IsMonster(void);

public:
	operator int(void) { return m_nUID; };
	//	void operator=(int val) { m_nTypeAll = val; };

public:
	static u4 GetClass(u2 nUID);
	static u4 GetIDX(u4 nUID);

	static u4 GetTestIDX(uint nClass);

public:
	static u4 GetMinUID(uint nClass);
	static u4 GetMaxUID(uint nClass);

public:
	void SetIDX(uint nIDX);				// Não use exceto em casos especiais
	void SetIDX(u4 nIDX, u4 nClass);	// usar na masmorra
	void SetType(uint nType);
	u4	 GetTypeAll(void);

public:
	enum
	{
		BitsIDX = 14, // 각 class별 object수 제한  0 ~ 4095    idx
		BitsClass = 5, // nClass 참조				 0 ~ 7       class
		BitsType = 19, // nClass 내의 해당넘의 종류 0 ~ 131,071 type

		BitsUID = 19, // BitsIDX:BitsClass
		BitsNULL = 19,
	};

	///////////////////////////////////////////////////////////////////
	// internal data
public:
#pragma warning( disable : 4201 )
	union
	{
		struct
		{
			u4 m_nIDX : BitsIDX;		// 각 class별 object수 제한  0 ~ 4095    idx
			u4 m_nClass : BitsClass;	// nClass 참조				 0 ~ 7       class
			u4 m_nType : BitsType;	// nClass 내의 해당넘의 종류 0 ~ 131,071 type
		}; // struct

		struct
		{
			u4 m_nUID : BitsUID;	// 게임내 unique id 0 ~ 32767
		u4: BitsNULL;
		}; // struct

		u8 m_nTypeAll;

	}; // union
}; // class cType





template <>
static inline void cPacket::push<cType>(cType& val)
{
	//xstop1(m_pIDX-m_pAll < MaxBufSize-32, "(T val), size:%d", m_pIDX-m_pAll);
	//xassert(m_nDebugState == SEND, "current state is not SEND");

	u4 data = val.GetTypeAll();
	push(data);

	//memcpy(m_pIDX, &data, sizeof(data));
	//m_pIDX += sizeof(data);
}

template <>
static inline void cPacket::pop<cType>(cType& val)
{
	u4 data = 0;
	//xassert(m_nDebugState == RECV, "current state is not RECV");
	//xassert2(m_pIDX+sizeof(data)<m_pEND, "current-end=%d size:%d", m_pIDX-m_pEND, sizeof(data));	

	//memcpy(&data, m_pIDX, sizeof(data));
	//m_pIDX += sizeof(data);

	pop(data);

	val.SetTypeAll(data);

}
