

#pragma once


#include "pLimit.h"
#include "nlib/base.h"

class cItemSlot // Informações usadas para db io
{
public:
	cItemSlot(void) : idx(0), maker(0) { }
   ~cItemSlot(void) { }

public:
	n8 idx;		// db idx do item
	n4 maker;	// Este construtor de item
};

#pragma pack(push)
#pragma pack(4)

class cItemData
{
public:
	cItemData(void) { Clear(); }
   ~cItemData(void) { }

public:
	uint GetType(void) const  { return m_nType; }
	uint GetCount(void) const { return m_nCount; }

	void SetType(uint nType);
	void SetCount(uint nCount);

	void Clear(void) { memset(this, 0, sizeof(cItemData)); }
	void Set( int nType, int nCount ){ m_nType = nType; m_nCount = nCount; m_nRate = 100; m_nLevel = 1; }
	void Set( int nType, int nCount, int nRate, int nLevel ){ m_nType = nType; m_nCount = nCount; m_nRate = nRate; m_nLevel = nLevel; }
	
public:
	//void operator=(cItemData &x) { memcpy(this, &x, sizeof(x)); }
	void operator=( cItemData const &x) { memcpy(this, &x, sizeof(x)); }

public:
	bool IsNoActive() { return (m_nEndTime == 0xFFFFFFF0) ? true : false;}  
	bool IsActive() 
	{
		if (m_nEndTime != 0 && m_nEndTime != 0xFFFFFFFF && m_nEndTime != 0xFFFFFFF0)
			return true;
		else
			return false;
	}
	void SetEndTime(u4 time) { m_nEndTime = time; }
	u4   GetEndTime()        { return m_nEndTime;}

public: // Informação básica
	union
	{
		struct
		{
			u4 m_nType      : 17; // Tipo de item (ctype :: m_ntype), 0 não é usado.
			u4 m_nCount     : 20; // Número de itens de pilha
		};

		struct
		{
			u4              : 20;
			u4 m_nEndurance : 20; // Durabilidade
		};

		u4 m_nAll;
	};

public: // Informações adicionais
	u1 m_nRate;		// Estatísticas
	u1 m_nLevel;	// Use quando o item fortalece
	u2 m_nLimited;	// Se você é atribuível, 1. Se você colocar um soquete, pertence, 2.

	u2 m_nSockItemType[nLimit::SocketSlot]; // Itemidx (dm_item_list.xls -Item -itemno) de itens de soquete empilhados, 1 ~ 65535
	u1 m_nSockAppRate[nLimit::SocketSlot];	// Taxa de aplicação

//#ifdef JHB_ADD_ACCESSORYSYSTEM_20131028	
	u2 m_nAccOption[nLimit::MAX_ACCESSORY_OPTION];	//8 opções de acessórios
	u2 m_nAccValues[nLimit::MAX_ACCESSORY_OPTION];	//8 acessórios
//#endif
public:
	u4 m_nEndTime;	// Superficialidade
	u4 m_nRemainTradeLimitTime;

#ifdef COMPAT_487
	u4 ExtraBytes;
#endif

//	pragma pack(4) Consideração do tratamento relacionado a 4 bytes
};
//#ifdef JHB_ADD_ACCESSORYSYSTEM_20131028
struct stAccessory
{
	u4 nSkillCode;		// Código de habilidade de acessórios
	u1 nGainCnt;		// Número de opções
	u1 nEnchantCnt;		// Contagem de uso do pedido
	//u1 nOptionNo;		// 1 poder de ataque, 2 defesa, 3 hp máximo, 4 ds máximos, 5 skilldem irrigação, 6 propriedades inadimplentes, 7 críticas
	u2 nOption[nLimit::MAX_ACCESSORY_EXCELOPTION];		// Leia as opções da folha de acessórios_option e salve -a
	u2 nMinValue[nLimit::MAX_ACCESSORY_EXCELOPTION];	// O valor mínimo da opção correspondente
	u2 nMaxValue[nLimit::MAX_ACCESSORY_EXCELOPTION];	// Valor máximo desta opção
};
struct stAccEnchant
{
	u4 nSkillCode;		// Aperfeiçoamento Número de habilidade do item de pedra de pedra
	u1 nEnchantOption;	// Opções encantadoras 1 energia digital, 2 renovação, 3 opções, 4 números mudam
	u2 SuccessPer;		// Probabilidade de sucesso -taxa de pagamento
	u2 SucValue;		// Aumento do sucesso
	u2 FailPer;			// Probabilidade de falha -taxa de porsonalidade
	u2 FailValue;		// Soltar valor quando falha
};
//#endif
typedef struct _stItemInfo
{
	u2 slotNo;
	n4 slotType;
	u4 idx;
	u4 cnt;
} stItemInfo;

#pragma pack(pop)






